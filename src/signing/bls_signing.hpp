#pragma once

#include "signing.hpp"

#include <bls/bls384_256.h>

#include <array>
#include <string_view>

// This file contains the logic for generating private keys, getting the public keys from them,
// signing messages and verifying that a signer (or group of signers for multi-sig) signed them using BLS.

template <>
class PublicKey<SigType::bls> {
public:
	blsPublicKey pub;
};

template <>
class SecretKey<SigType::bls> {
public:
	PublicKey<SigType::bls> GetPublicKey() const;
	static SecretKey<SigType::bls> Generate();

	blsSecretKey key;
};

template <>
class Signature<SigType::bls> {
public:
	bool Verify(PublicKey<SigType::bls> const& publicKey, std::string_view msg) const;

	template <class T>
	bool VerifyAggregate(T const& publicKeys, std::string_view msg) const {
		return blsFastAggregateVerify(
						 &sig, reinterpret_cast<const blsPublicKey*>(publicKeys.data()), publicKeys.size(), msg.data(), msg.size()) == 1;
	}

	blsSignature sig;
};

struct MultiSigId {
	blsId id;
};

template <size_t Total, size_t Min>
auto MultiSigSharedKeyGenerator(
	std::array<SecretKey<SigType::bls>, Min> const& secretKeyMask, std::array<MultiSigId, Total> const& ids) {
	// Distributed secret keys
	std::array<SecretKey<SigType::bls>, Total> keys;
	for (auto i = 0u; i < Total; i++) {
		blsSecretKeyShare(reinterpret_cast<blsSecretKey*>(&keys[i]), reinterpret_cast<const blsSecretKey*>(secretKeyMask.data()), Min,
			reinterpret_cast<const blsId*>(&ids[i]));
	}
	return keys;
}

template <typename T>
concept HasDataSize = requires(T t) {
	t.data();
	t.size();
};

template <typename T>
requires HasDataSize<T>&& std::is_standard_layout_v<T> Signature<SigType::bls> AggregateSignatures(T const& signatures) {
	blsSignature aggSig;
	blsAggregateSignature(&aggSig, reinterpret_cast<const blsSignature*>(signatures.data()), signatures.size());
	return {aggSig};
}
