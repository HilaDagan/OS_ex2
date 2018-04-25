cmake_minimum_required(VERSION 3.7)
project(tesmp)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Thread.cpp)
add_executable(tesmp ${SOURCE_FILES})