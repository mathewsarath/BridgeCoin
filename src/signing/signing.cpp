#include "signing.hpp"

#include <array>
#include <stdexcept>

using namespace std::string_view_literals;

void InitSigLib() {
	auto res = blsInit(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);
	if (res != 0) {
		throw std::runtime_error("BLS failed to initialize");
	}
}

PublicKey SecretKey::GetPublicKey() const {
	blsPublicKey pub;
	blsGetPublicKey(&pub, &key);
	return {pub};
}

SecretKey SecretKey::Generate() {
	blsSecretKey key;
	blsSecretKeySetByCSPRNG(&key);
	return {key};
}

bool Signature::Verify(PublicKey const& publicKey, std::string_view msg) const {
	return blsVerify(&sig, &publicKey.pub, msg.data(), msg.size()) == 1;
}

Signature Sign(SecretKey const& secretKey, std::string_view msg) {
	blsSignature sig;
	blsSign(&sig, &secretKey.key, msg.data(), msg.size());
	return {sig};
}
