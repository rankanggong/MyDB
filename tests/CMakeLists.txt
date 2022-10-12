cmake_minimum_required(VERSION 3.17)
project(tests)
add_executable(tests storage/rocksdb_keyvaluestore_test.cpp)
target_link_libraries(tests gtest src)