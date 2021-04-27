add_executable(tests tests.cpp config.cpp signing.cpp hashing.cpp ledger_store.cpp)
target_link_libraries(
  tests
  PRIVATE project_warnings
          project_options
          signing
          algorithms
          config
          debugging
          hashing
          ledger_store
          blocks
          bls::bls384_256
          CONAN_PKG::gtest
          CONAN_PKG::fmt)

enable_testing()
add_test(
  NAME "allTests"
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  COMMAND tests)
