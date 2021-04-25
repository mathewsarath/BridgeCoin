add_subdirectory(algorithms)
add_subdirectory(config)
add_subdirectory(signing)

add_executable(node main.cpp)
target_link_libraries(
  node
  PRIVATE project_options
          project_warnings
          CONAN_PKG::fmt
          CONAN_PKG::spdlog
          signing
          algorithms
          config
          Secp256k1)
