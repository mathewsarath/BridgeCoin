#include "blocks/blocks.hpp"
#include "config/paths.hpp"
#include "ledger_store/rocksdb_ledger_store.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>

TEST(LedgerStore, Block) {
	auto path = WorkingPath() / fmt::format("store{}", std::chrono::system_clock::now().time_since_epoch().count());
	RocksDBLedgerStore store(path);

	SendBlock block;
	block.blockHeight = 1;
	auto hash = block.GenerateHash();
	{
		auto transaction = store.TxBeginWrite();
		store.SendBlockPut(transaction, block);

		auto val = store.Get(transaction, Document::blocks, hash);
		const auto block1 = reinterpret_cast<const SendBlock*>(val.Data());
		ASSERT_EQ(block, *block1);
	}

	auto transaction = store.TxBeginRead();
	auto val = store.Get(transaction, Document::blocks, hash);
	const auto block1 = reinterpret_cast<const SendBlock*>(val.Data());
	ASSERT_EQ(block, *block1);
}