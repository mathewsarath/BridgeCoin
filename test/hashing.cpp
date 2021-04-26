#include "hashing/hashing.hpp"

#include "core/units.hpp"

#include <gtest/gtest.h>

TEST(Hashing, Simple) {
	auto toHash = std::initializer_list<uint8_t>{1, 2, 3};
	Hash out;
	{
		Hashable<sizeof(Hash)> hashable;
		hashable.Update(toHash);
		hashable.Update(toHash);
		out = hashable.Output();
	}

	std::array<uint8_t, 32> expectedHash{189, 39, 169, 16, 195, 170, 204, 234, 240, 201, 79, 140, 169, 143, 79, 219, 243, 195, 94,
		227, 135, 255, 75, 110, 175, 23, 41, 142, 238, 23, 11, 46};
	ASSERT_EQ(out, expectedHash);

	{
		// Do the same again and the outputs should be identical
		Hashable<sizeof(Hash)> hashable;
		hashable.Update(toHash);
		hashable.Update(toHash);
		ASSERT_EQ(out, hashable.Output());
	}

	{
		// Hashing different values should give different results
		Hashable<sizeof(Hash)> hashable;
		hashable.Update(toHash);
		ASSERT_NE(out, hashable.Output());
	}
}