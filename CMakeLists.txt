cmake_minimum_required(VERSION 3.10)
project(myTinySTL)
set(CMAKE_CXX_FLAGS "-g -Wall")
set(CMAKE_CXX_STANDARD 14)
include_directories(./)

add_executable(STL_test1 alloc.cpp test/testVector.cpp)

add_executable(STL_test2 alloc.cpp test/testList.cpp)

add_executable(STL_test3 alloc.cpp test/testSet.cpp)

add_executable(STL_test4 alloc.cpp test/testMap.cpp)

#target_link_libraries(STL_test pthread)
