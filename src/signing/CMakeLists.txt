add_library(signing bls_signing.hpp bls_signing.cpp signing.hpp)
target_link_libraries(
  signing
  PRIVATE project_options
          project_warnings
          algorithms
          bls::bls384_256)

target_compile_definitions(signing PRIVATE BLS_DONT_EXPORT)
