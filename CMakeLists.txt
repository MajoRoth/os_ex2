cmake_minimum_required(VERSION 3.20.2)
project(os_ex2)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(os_ex2
        Thread.cpp
        Thread.h
        uthreads.cpp
        uthreads.h
        tests/ex_2_tests_updated/tests/test3.in.cpp)
#        main.cpp)

