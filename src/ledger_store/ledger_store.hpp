#pragma once

#include "algorithms/concepts.hpp"
#include "blocks/blocks.hpp"

#include <cassert>
#include <cstdint>

enum class Backend {	// Just a tag
	rocksdb
};

template <Backend backend>
class WriteTransaction {};

template <Backend backend>
class ReadTransaction {};

template <typename PinnableVal>
class DBReadVal {
public:
	// Must be specialized
	const void* Data() const;

	// For rocksdb if this is in the block cache then it will just be data->size otherwise will contain a copy
	PinnableVal slice;
};

template <typename Val>
class DBVal {
public:
	// Must be specialized
	const void* Data() const;
	size_t Size() const;
	DBVal(size_t size, const void* data);

	DBVal(std::nullptr_t) : DBVal(0, this) {
	}

	// Prevent passing in r-values as scope should be extended beyond this call
	template <typename T>
	DBVal(const T&&) = delete;

	template <typename T>
	DBVal(const T& val) requires HasDataSize<T> : DBVal(std::size(val), static_cast<const void*>(std::data(val))) {
	}

	// Passing by reference to ensure that the values exist after the call is finished
	DBVal(const uint64_t& val) : DBVal(sizeof(val), static_cast<const void*>(&val)) {
	}

	operator const Val*() const {
		return &value;
	}

	operator const Val&() const {
		return value;
	}

	Val value;
};

enum class Document { blocks };

template <class Val, class PinnableVal, typename DerivedStore, typename Status, Backend backend>
class LedgerStore {
public:
	void Initialize(const WriteTransaction<backend>& transaction) {
		static_cast<const DerivedStore*>(this)->Initialize(transaction);
	}

	void SendBlockPut(const WriteTransaction<backend>& transaction, const SendBlock& block) {
		DBVal<Val> value{sizeof(block), static_cast<const void*>(&block)};
		auto hash = block.GenerateHash();
		auto status = Put(transaction, Document::blocks, hash, value);
		assert(Success(status));
	}

	template <typename Transaction>
	DBReadVal<PinnableVal> Get(const Transaction& transaction, Document document, DBVal<Val> const& key) {
		return static_cast<DerivedStore&>(*this).Get(transaction, document, key);
	}

	Status Put(WriteTransaction<backend> const& transaction, Document document, DBVal<Val> const& key, DBVal<Val> const& value) {
		return static_cast<DerivedStore&>(*this).Put(transaction, document, key, value);
	}

	// Put only key without value
	Status PutKey(WriteTransaction<backend> const& transaction, Document document, DBVal<Val> const& key) {
		return Put(transaction, document, key, DBVal<Val>{nullptr});
	}

	bool Success(Status status) {
		return static_cast<DerivedStore&>(*this).Success(status);
	}

	WriteTransaction<backend> TxBeginWrite() {
		return static_cast<DerivedStore&>(*this).TxBeginWrite();
	}

	ReadTransaction<backend> TxBeginRead() const {
		return static_cast<DerivedStore&>(*this).TxBeginRead();
	}
};