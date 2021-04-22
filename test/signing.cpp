#include <gtest/gtest.h>

#include <array>
#include <signing/signing.hpp>

using namespace std::string_view_literals;

TEST(Signing, Basic) {
	InitSigLib();

	// Generate a private key, sign a message and check that verification works
	auto key = SecretKey::Generate();
	auto pub = key.GetPublicKey();
	auto msg = "abc"sv;
	auto signature = Sign(key, msg);
	ASSERT_TRUE(signature.Verify(pub, msg));

	// Check that verification fails with a different message
	ASSERT_FALSE(signature.Verify(pub, "wrong message"));
}

TEST(Signing, Aggregate) {
	const auto numSigs = 2u;
	auto msg = "abc"sv;
	auto [keys, pubs] = GenKeyPairs<numSigs>();
	std::array<Signature, numSigs> sigs;
	for (auto i = 0u; i < numSigs; ++i) {
		sigs[i] = Sign(keys[i], msg);
	}

	// Aggregate signatures
	auto aggSignature = AggregateSignatures(sigs);
	ASSERT_TRUE(aggSignature.VerifyAggregate(pubs, msg));
	ASSERT_FALSE(aggSignature.VerifyAggregate(pubs, "wrong message"));
}

TEST(Signing, Multisig) {
	// A minimum number of keys is needed to sign a transaction using multisig
	size_t const total = 5u;
	auto const min = 3u;

	// All ids must be non-zero and different from each other.
	std::array<MultiSigId, total> ids;
	for (auto i = 0u; i < total; ++i) {
		blsIdSetInt(reinterpret_cast<blsId*>(&ids[i]), static_cast<int>(i + 1u));
	}

	auto secretKeyMask = GenSecretKeys<min>();
	auto sharedKeys = MultiSigSharedKeyGenerator(secretKeyMask, ids);

	// Get master public key
	auto masterPublicKey = secretKeyMask.front().GetPublicKey();

	// Each user gets their own public key
	std::array<PublicKey, total> pubs;
	for (auto i = 0u; i < total; ++i) {
		pubs[i] = sharedKeys[i].GetPublicKey();
	}

	// Each user signs the message
	auto msg = "abc"sv;
	std::array<Signature, total> sigs;
	for (auto i = 0u; i < total; ++i) {
		sigs[i] = Sign(sharedKeys[i], msg);
	}

	// Recover the signatures
	Signature sig;
	blsSignatureRecover(reinterpret_cast<blsSignature*>(&sig), reinterpret_cast<const blsSignature*>(sigs.data()),
		reinterpret_cast<const blsId*>(ids.data()), min);
	ASSERT_TRUE(sig.Verify(masterPublicKey, msg));

	// Recover using an offset from the beginning
	blsSignatureRecover(reinterpret_cast<blsSignature*>(&sig), reinterpret_cast<const blsSignature*>(sigs.data() + 1),
		reinterpret_cast<const blsId*>(ids.data() + 1), min);
	ASSERT_TRUE(sig.Verify(masterPublicKey, msg));

	// any min-1 of N sigs can't recover
	blsSignatureRecover(reinterpret_cast<blsSignature*>(&sig), reinterpret_cast<const blsSignature*>(sigs.data()),
		reinterpret_cast<const blsId*>(ids.data()), min - 1);
	ASSERT_FALSE(sig.Verify(masterPublicKey, msg));
}