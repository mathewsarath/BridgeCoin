#include <spdlog/spdlog.h>

int main(int, const char**) {
	spdlog::info("Hello, {}!", "World");
}
