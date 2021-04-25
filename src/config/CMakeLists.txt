add_library (config
	paths.cpp
	paths.hpp)

target_link_libraries(
  config
  PRIVATE project_options
          project_warnings
          CONAN_PKG::boost)