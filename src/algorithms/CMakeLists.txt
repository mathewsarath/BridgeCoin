add_library(algorithms INTERFACE)
add_custom_target(algorithms-target SOURCES "compile_time_operations.hpp" "concepts.hpp")
