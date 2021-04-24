#include "bls_signing.hpp"

#include <array>
#include <stdexcept>

using namespace std::string_view_literals;

template <>
void InitSigLib<SigType::bls>() {
	auto res = blsInit(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);
	if (res != 0) {
		throw std::runtime_error("BLS failed to initialize");
	}
}

PublicKey<SigType::bls> SecretKey<SigType::bls>::GetPublicKey() const {
	blsPublicKey pub;
	blsGetPublicKey(&pub, &key);
	return {pub};
}

SecretKey<SigType::bls> SecretKey<SigType::bls>::Generate() {
	blsSecretKey key;
	blsSecretKeySetByCSPRNG(&key);
	return {key};
}

bool Signature<SigType::bls>::Verify(PublicKey<SigType::bls> const& publicKey, std::string_view msg) const {
	return blsVerify(&sig, &publicKey.pub, msg.data(), msg.size()) == 1;
}

template <>
Signature<SigType::bls> Sign(SecretKey<SigType::bls> const& secretKey, std::string_view msg) {
	blsSignature sig;
	blsSign(&sig, &secretKey.key, msg.data(), msg.size());
	return {sig};
}
