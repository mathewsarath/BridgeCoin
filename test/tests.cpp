#include "debugging/stacktrace.hpp"

#include <gtest/gtest.h>

#include <csignal>

namespace {
void SignalHandler(int signum) {
	std::signal(signum, SIG_DFL);
	auto stacktrace = GenerateStacktrace();
	std::cerr << stacktrace << std::endl;
}
}	 // namespace

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	std::signal(SIGSEGV, &SignalHandler);

	return RUN_ALL_TESTS();
}
