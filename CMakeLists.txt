cmake_minimum_required(VERSION 3.22)
project(os_ex2)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(os_ex2
        Thread.cpp
        Thread.h
        uthreads.cpp
        uthreads.h
        main.cpp)

