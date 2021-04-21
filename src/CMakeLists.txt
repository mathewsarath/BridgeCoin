add_executable(node main.cpp)
target_link_libraries(
  node
  PRIVATE project_options
          project_warnings
          CONAN_PKG::fmt
          CONAN_PKG::spdlog
          bls::bls384_256)

target_compile_definitions(node PRIVATE BLS_DONT_EXPORT)