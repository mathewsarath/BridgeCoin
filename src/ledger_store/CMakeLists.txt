add_library(ledger_store ledger_store.hpp rocksdb_ledger_store.cpp rocksdb_ledger_store.hpp)
target_link_libraries(
  ledger_store
  PRIVATE project_options
          project_warnings
  PUBLIC CONAN_PKG::rocksdb)
