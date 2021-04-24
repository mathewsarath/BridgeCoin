template <auto... Xs, typename F>
constexpr void for_values(F&& f) {
	(f.template operator()<Xs>(), ...);
}