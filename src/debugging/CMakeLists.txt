add_library(debugging stacktrace.cpp stacktrace.hpp)
target_link_libraries(
  debugging
  PRIVATE project_options
          project_warnings
          CONAN_PKG::boost
          ${CMAKE_DL_LIBS})
