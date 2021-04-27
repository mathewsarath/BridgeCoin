#pragma once

#include "ledger_store.hpp"

#include <rocksdb/db.h>
#include <rocksdb/filter_policy.h>
#include <rocksdb/options.h>
#include <rocksdb/slice.h>
#include <rocksdb/status.h>
#include <rocksdb/utilities/optimistic_transaction_db.h>
#include <rocksdb/utilities/transaction.h>
#include <rocksdb/utilities/transaction_db.h>

#include <filesystem>
#include <thread>

using RocksDBVal = DBVal<rocksdb::Slice>;
using RocksDBPinnableVal = DBReadVal<rocksdb::PinnableSlice>;

template <>
class WriteTransaction<Backend::rocksdb> {
public:
	WriteTransaction(rocksdb::OptimisticTransactionDB* optimisticDB) : db(optimisticDB) {
		rocksdb::OptimisticTransactionOptions txnOptions;
		txnOptions.set_snapshot = true;
		txn.reset(db->BeginTransaction(rocksdb::WriteOptions(), txnOptions));
	}

	~WriteTransaction() {
		commit();
	}

	void commit() {
		auto status = txn->Commit();

		// If there are no available memtables try again a few more times
		constexpr auto numAttempts = 10;
		auto attemptNum = 0;
		while (status.IsTryAgain() && attemptNum < numAttempts) {
			status = txn->Commit();
			++attemptNum;
		}
		// Should be a fatal error....
		assert(status.ok());	//, status.ToString());
	}

	void renew() {
		rocksdb::OptimisticTransactionOptions txnOptions;
		txnOptions.set_snapshot = true;
		db->BeginTransaction(rocksdb::WriteOptions(), txnOptions, txn.get());
	}

	rocksdb::Transaction* GetHandle() const {
		return txn.get();
	}

	std::unique_ptr<rocksdb::Transaction> txn;
	rocksdb::OptimisticTransactionDB* db;
};

template <>
class ReadTransaction<Backend::rocksdb> {
public:
	ReadTransaction(rocksdb::DB* _db) : db(_db) {
		if (db) {
			options.snapshot = db->GetSnapshot();
		}
	}

	~ReadTransaction() {
		Reset();
	}

	void Reset() {
		if (db) {
			db->ReleaseSnapshot(options.snapshot);
		}
	}

	void Renew() {
		options.snapshot = db->GetSnapshot();
	}

	const void* GetHandle() const {
		return const_cast<void*>(static_cast<const void*>(&options));
	}

private:
	rocksdb::DB* db;
	rocksdb::ReadOptions options;
};

namespace {
inline const rocksdb::ReadOptions& SnapshotOptions(ReadTransaction<Backend::rocksdb> const& transaction) {
	return *static_cast<const rocksdb::ReadOptions*>(transaction.GetHandle());
}
}	 // namespace

class RocksDBLedgerStore
	: public LedgerStore<rocksdb::Slice, rocksdb::PinnableSlice, RocksDBLedgerStore, rocksdb::Status, Backend::rocksdb> {
public:
	RocksDBLedgerStore(std::filesystem::path const& path) {
		if (!std::filesystem::exists(path)) {
			std::filesystem::create_directories(path);
		}

		auto options = GetDBOptions();
		std::vector<rocksdb::ColumnFamilyHandle*> handles;
		rocksdb::Status s =
			rocksdb::OptimisticTransactionDB::Open(options, path.string(), CreateColumnFamilies(), &handles, &optimisticDB);
		if (!s.ok()) {
			throw std::runtime_error(s.ToString());
		}

		if (optimisticDB) {
			db.reset(optimisticDB);
		}

		for (auto handle : handles) {
			if (handle->GetName() == "blocks") {
				blocksHandle.reset(handle);
			} else if (handle->GetName() == rocksdb::kDefaultColumnFamilyName) {
				// Delete this now, so we don't need to worry about it during destruction, calling db->DefaultColumnFamily () doesn't return
				// same pointer..
				delete handle;
			} else {
				assert(false);
			}
		}
	}

	std::vector<rocksdb::ColumnFamilyDescriptor> CreateColumnFamilies() {
		std::vector<rocksdb::ColumnFamilyDescriptor> columnFamilies;
		for ([[maybe_unused]] auto& [cfName, table] : CreateCFNameDocumentMap()) {
			columnFamilies.emplace_back(cfName, GetCFOptions(cfName));
		}
		return columnFamilies;
	}

	void Initialize(const WriteTransaction<Backend::rocksdb>& writeTransaction);

	rocksdb::ColumnFamilyHandle* DocumentToColumnFamily(Document document) const {
		switch (document) {
			case Document::blocks:
				return blocksHandle.get();
		}

		throw std::runtime_error("Document does not exist");
	}

	template <typename Transaction>
	rocksdb::Transaction* tx(Transaction const& transaction) const {
		return transaction.GetHandle();
	}

	template <typename Transaction>
	RocksDBPinnableVal Get(const Transaction& transaction, Document document, RocksDBVal key) {
		rocksdb::ReadOptions options;
		RocksDBPinnableVal val;
		auto handle = DocumentToColumnFamily(document);
		rocksdb::Status status;
		if constexpr (std::is_same_v<Transaction, ReadTransaction<Backend::rocksdb>>) {
			status = db->Get(SnapshotOptions(transaction), handle, key, &val.slice);
		} else {
			status = tx(transaction)->Get(options, handle, key, &val.slice);
		}

		assert(Success(status));
		return val;
	}

	rocksdb::Status Put(
		WriteTransaction<Backend::rocksdb> const& transaction, Document document, RocksDBVal const& key, RocksDBVal const& value) {
		//	assert(transaction.contains(document));
		auto txn = tx(transaction);
		return txn->Put(DocumentToColumnFamily(document), key, value);
	}

	// Put only key without value
	rocksdb::Status PutKey(WriteTransaction<Backend::rocksdb> const& transaction, Document document, RocksDBVal const& key) {
		return Put(transaction, document, key, RocksDBVal{nullptr});
	}

	bool Success(rocksdb::Status status) {
		return status.ok();
	}

	rocksdb::ColumnFamilyOptions GetCFOptions(std::string const& cfName) const {
		rocksdb::ColumnFamilyOptions cfOptions;
		if (cfName == "blocks") {
		} else if (cfName == rocksdb::kDefaultColumnFamilyName) {
			// Do nothing.
		} else {
			assert(false);
		}

		return cfOptions;
	}

	rocksdb::Options GetDBOptions() const {
		rocksdb::Options dbOptions;
		dbOptions.create_if_missing = true;
		dbOptions.create_missing_column_families = true;

		// Enable whole key bloom filter in memtables for ones with memtable_prefix_bloom_size_ratio set
		// It can potentially reduce CPU usage for point-look-ups.
		dbOptions.memtable_whole_key_filtering = true;

		// Sets the compaction priority
		dbOptions.compaction_pri = rocksdb::CompactionPri::kMinOverlappingRatio;

		// Start aggressively flushing WAL files when they reach over 1GB
		dbOptions.max_total_wal_size = 1 * 1024 * 1024 * 1024LL;

		// Optimize RocksDB. This is the easiest way to get RocksDB to perform well
		dbOptions.IncreaseParallelism(static_cast<int>(std::thread::hardware_concurrency()));
		dbOptions.OptimizeLevelStyleCompaction();

		// Adds a separate write queue for memtable/WAL
		dbOptions.enable_pipelined_write = true;

		// Default is 16, setting to -1 allows faster startup times for SSDs by allowings more files to be read in parallel.
		dbOptions.max_file_opening_threads = -1;

		// The MANIFEST file contains a history of all file operations since the last time the DB was opened and is replayed during DB
		// open. Default is 1GB, lowering this to avoid replaying for too long (100MB)
		dbOptions.max_manifest_file_size = 100 * 1024 * 1024ULL;

		// Not compressing any SST files for compatibility reasons.
		dbOptions.compression = rocksdb::kNoCompression;

		return dbOptions;
	}

	std::unordered_map<const char*, Document> CreateCFNameDocumentMap() const {
		// Just using blocks for default name as it gets deleted later anyway
		std::unordered_map<const char*, Document> map{
			{rocksdb::kDefaultColumnFamilyName.c_str(), Document::blocks}, {"blocks", Document::blocks}};

		return map;
	}

	WriteTransaction<Backend::rocksdb> TxBeginWrite() {
		assert(optimisticDB != nullptr);
		return WriteTransaction<Backend::rocksdb>(optimisticDB);
	}

	ReadTransaction<Backend::rocksdb> TxBeginRead() {
		assert(optimisticDB != nullptr);
		return ReadTransaction<Backend::rocksdb>(db.get());
	}

	rocksdb::OptimisticTransactionDB* optimisticDB = nullptr;
	std::unique_ptr<rocksdb::DB> db;

	std::unique_ptr<rocksdb::ColumnFamilyHandle> blocksHandle;
};