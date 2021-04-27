#pragma once

#include "algorithms/concepts.hpp"

#include <libb2/blake2.h>

#include <array>

template <size_t DigestSize>
class Hashable {
public:
	Hashable() {
		blake2b_init(&hasher, DigestSize);
	}

	template <typename T>
	void Update(const T& buf) requires HasDataSize<T> {
		blake2b_update(&hasher, std::data(buf), std::size(buf));
	}

	template <typename T>
	void Update(const T& buf) requires std::integral<T> {
		blake2b_update(&hasher, reinterpret_cast<const void*>(&buf), sizeof(buf));
	}

	std::array<uint8_t, DigestSize> Output() {
		std::array<uint8_t, DigestSize> hash;
		blake2b_final(&hasher, &hash, DigestSize);
		return hash;
	}

private:
	blake2b_state hasher;
};