#pragma once

// This file contains the logic for generating private keys, getting the public keys from them,
// signing messages and verifying that a signer (or group of signers for multi-sig) signed them using BLS.

#include <bls/bls384_256.h>

#include <string_view>

void InitSigLib();

class PublicKey {
public:
	blsPublicKey pub;
};

class SecretKey {
public:
	PublicKey GetPublicKey() const;
	static SecretKey Generate();

	blsSecretKey key;
};

class Signature {
public:
	bool Verify(PublicKey const& publicKey, std::string_view msg) const;

	template <class T>
	bool VerifyAggregate(T const& publicKeys, std::string_view msg) const {
		return blsFastAggregateVerify(
						 &sig, reinterpret_cast<const blsPublicKey*>(publicKeys.data()), publicKeys.size(), msg.data(), msg.size()) == 1;
	}

	blsSignature sig;
};

Signature Sign(SecretKey const& secretKey, std::string_view msg);

struct KeyPair {
	SecretKey key;
	PublicKey pub;
};

template <int Num>
auto GenSecretKeys() {
	std::array<SecretKey, Num> secretKeys;
	for (auto& key : secretKeys) {
		key = SecretKey::Generate();
	}
	return secretKeys;
}

template <int Num>
auto GenKeyPairs() {
	std::array<SecretKey, Num> keys;
	std::array<PublicKey, Num> pubs;
	for (auto i = 0; i < Num; ++i) {
		keys[i] = SecretKey::Generate();
		pubs[i] = keys[i].GetPublicKey();
	}
	return std::pair{keys, pubs};
}

struct MultiSigId {
	blsId id;
};

template <int Total, int Min>
auto MultiSigSharedKeyGenerator(std::array<SecretKey, Min> const& secretKeyMask, std::array<MultiSigId, Total> const& ids) {
	/*
		A trusted third party distributes num secret keys.
		If you want to avoid it, then see DKG (distributed key generation),
		which is out of the scope of this library.
	*/

	// Distributed secret keys
	std::array<SecretKey, Total> keys;
	for (int i = 0; i < Total; i++) {
		blsSecretKeyShare(reinterpret_cast<blsSecretKey*>(&keys[i]), reinterpret_cast<const blsSecretKey*>(secretKeyMask.data()), Min,
			reinterpret_cast<const blsId*>(&ids[i]));
	}
	return keys;
}

template <typename T>
concept HasDataSize = requires {
	T::data;
	T::size;
};

template <class T>
requires HasDataSize<T>&& std::is_standard_layout_v<T> Signature AggregateSignatures(T const& signatures) {
	blsSignature aggSig;
	blsAggregateSignature(&aggSig, reinterpret_cast<const blsSignature*>(signatures.data()), signatures.size());
	return {aggSig};
}