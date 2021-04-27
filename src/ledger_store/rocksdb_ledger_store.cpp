#include "rocksdb_ledger_store.hpp"

#include <filesystem>

template <>
const void* RocksDBVal::Data() const {
	return static_cast<const void*>(value.data());
}

template <>
size_t RocksDBVal::Size() const {
	return value.size();
}

template <>
RocksDBVal::DBVal(size_t size, const void* data) : value(static_cast<const char*>(data), size) {
}

template <>
const void* RocksDBPinnableVal::Data() const {
	return static_cast<const void*>(slice.data());
}

using RocksDBWriteTransaction = WriteTransaction<Backend::rocksdb>;

void RocksDBLedgerStore::Initialize(const RocksDBWriteTransaction&) {
}