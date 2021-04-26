#include <array>
#include <concepts>
#include <initializer_list>

template <typename T>
concept HasDataSize = requires(T t) {
	std::data(t);
	std::size(t);
};
