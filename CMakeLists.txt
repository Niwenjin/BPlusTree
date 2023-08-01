cmake_minimum_required(VERSION 3.5)

project("BPlusTree")

include_directories(include)

SET(CMAKE_BUILD_TYPE "Debug")

add_executable(bpt main.cpp src/bpt.cpp)

target_link_libraries(bpt readline)