add_library(blocks blocks.cpp blocks.hpp)
target_link_libraries(
  blocks
  PRIVATE project_options
          project_warnings
          hashing)
