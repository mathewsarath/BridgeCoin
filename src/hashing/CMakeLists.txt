add_library(hashing hashing.cpp hashing.hpp)
target_link_libraries(
  hashing
  PRIVATE project_options
          project_warnings          
  PUBLIC CONAN_PKG::libb2)
