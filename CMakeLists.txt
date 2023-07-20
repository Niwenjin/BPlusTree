cmake_minimum_required(VERSION 3.5)

project("BPlusTree")

include_directories(include)

add_executable(main main.cpp src/bpt.cpp)
