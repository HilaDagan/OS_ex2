cmake_minimum_required(VERSION 3.7)
project(OS_ex2)

set(CMAKE_CXX_STANDARD 11)

#set(SOURCE_FILES main.cpp Thread.cpp)
set(SOURCE_FILES main.cpp uthreads_test.cpp Thread.cpp)
#set(SOURCE_FILES demo_jmp.c)
#set(SOURCE_FILES demo_itimer.c)
#set(SOURCE_FILES main.cpp Thread.cpp uthreads.cpp uthreads_test.cpp)
add_executable(OS_ex2 ${SOURCE_FILES})