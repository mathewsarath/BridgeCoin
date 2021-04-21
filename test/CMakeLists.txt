add_executable(tests tests.cpp)
target_link_libraries(tests CONAN_PKG::gtest)

enable_testing()
add_test(
  NAME "allTests"
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  COMMAND tests)
