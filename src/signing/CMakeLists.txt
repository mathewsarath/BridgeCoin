add_library(signing signing.hpp signing.cpp)
target_link_libraries(
  signing
  PRIVATE project_options
          project_warnings
	  bls::bls384_256)

target_compile_definitions(signing PRIVATE BLS_DONT_EXPORT) 