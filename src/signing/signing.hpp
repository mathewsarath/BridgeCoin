#pragma once

#include <array>
#include <string_view>

enum class SigType {
	bls,
};

template <SigType>
void InitSigLib();

template <SigType>
class PublicKey;
template <SigType>
class SecretKey;
template <SigType>
class Signature;

template <SigType SigType>
struct KeyPair {
	SecretKey<SigType> key;
	PublicKey<SigType> pub;
};

template <SigType SigType, size_t Num>
auto GenSecretKeys() {
	std::array<SecretKey<SigType>, Num> secretKeys;
	for (auto& key : secretKeys) {
		key = SecretKey<SigType>::Generate();
	}
	return secretKeys;
}

template <SigType SigType>
Signature<SigType> Sign(SecretKey<SigType> const& secretKey, std::string_view msg);

template <SigType SigType, size_t Num>
auto GenKeyPairs() {
	std::array<SecretKey<SigType>, Num> keys;
	std::array<PublicKey<SigType>, Num> pubs;
	for (auto i = 0u; i < Num; ++i) {
		keys[i] = SecretKey<SigType>::Generate();
		pubs[i] = keys[i].GetPublicKey();
	}
	return std::pair{keys, pubs};
}
