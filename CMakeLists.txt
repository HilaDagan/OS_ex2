cmake_minimum_required(VERSION 3.7)
project(OS_ex2)

set(CMAKE_CXX_STANDARD 11)

#set(SOURCE_FILES main.cpp Thread.cpp)
#set(SOURCE_FILES main.cpp uthreads_test.cpp Thread.cpp)
set(SOURCE_FILES main.cpp uthreads.cpp Thread.cpp)
#set(SOURCE_FILES demo_jmp.c)
#set(SOURCE_FILES demo_itimer.c)
#set(SOURCE_FILES sync_test.cpp Thread.cpp uthreads.cpp)
add_executable(OS_ex2 ${SOURCE_FILES})

# Ori's- test
#set(Sync_Test sync_test.cpp uthreads.cpp uthreads.h Thread.cpp)
#add_executable(Sync_Test ${Sync_Test})