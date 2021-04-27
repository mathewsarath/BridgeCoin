add_subdirectory(algorithms)
add_subdirectory(blocks)
add_subdirectory(config)
add_subdirectory(debugging)
add_subdirectory(hashing)
add_subdirectory(ledger_store)
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
          ledger_store
          blocks
          Secp256k1
          CONAN_PKG::fmt
          CONAN_PKG::spdlog)
