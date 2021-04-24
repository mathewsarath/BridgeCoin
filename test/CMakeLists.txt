add_executable(tests tests.cpp signing.cpp)
target_link_libraries(
  tests
  PRIVATE project_warnings
          project_options
          signing
          algorithms
          bls::bls384_256
          CONAN_PKG::gtest)

enable_testing()
add_test(
  NAME "allTests"
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  COMMAND tests)
