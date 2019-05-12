cmake_minimum_required(VERSION 3.10)
project(myTinySTL)

set(CMAKE_CXX_STANDARD 14)
include_directories(./)

add_executable(STL_test alloc.cpp test/testVector.cpp)

#target_link_libraries(STL_test pthread)
