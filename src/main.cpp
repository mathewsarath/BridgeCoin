#include <bls/bls384_256.h>
#include <secp256k1_sha256.h>
#include <spdlog/spdlog.h>

int main(int, const char**) {
	[[maybe_unused]] blsPublicKey pub;

	secp256k1_sha256_t ctx;
	secp256k1_sha256_initialize(&ctx);

	spdlog::info("Hello, {}!", "World");
}
