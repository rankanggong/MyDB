# CMakeList.txt : CMake project for MyDB, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("MyDB")

SET(ZLIB_LIBRARY "C:/dev/vcpkg/installed/x64-windows/share/zlib")
SET(RocksDB_DIR "C:/dev/vcpkg/installed/x64-windows/share/rocksdb")
SET(fmt_DIR "C:/dev/vcpkg/installed/x64-windows/share/fmt")

find_package(RocksDB CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

# Add source to this project's executable.
add_executable (MyDB "src/storage/rocksdb_keyvaluestore.cpp" "src/storage/rocksdb_keyvaluestore.h" "src/model/status.h"  "src/model/config.cpp" "src/model/config.h")
target_link_libraries(MyDB PRIVATE RocksDB:rocksdb)
target_link_libraries(MyDB PRIVATE fmt::fmt)

# TODO: Add tests and install targets if needed.