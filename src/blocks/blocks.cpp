#include "blocks.hpp"

#include "hashing/hashing.hpp"

#include <cassert>

Hash SendBlock::GenerateHash() const {
	Hashable<sizeof(Hash)> hashable;
	hashable.Update(blockHeight);
	return hashable.Output();
}