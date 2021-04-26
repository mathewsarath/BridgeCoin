add_subdirectory(algorithms)
add_subdirectory(config)
add_subdirectory(debugging)
add_subdirectory(hashing)
add_subdirectory(signing)

add_executable(node main.cpp)
target_link_libraries(
  node
  PRIVATE project_options
          project_warnings
          signing
          config
          debugging
          algorithms
          hashing
          Secp256k1
          CONAN_PKG::fmt
          CONAN_PKG::spdlog)
