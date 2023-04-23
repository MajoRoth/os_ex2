cmake_minimum_required(VERSION 3.22)
project(os_ex2)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(os_ex2
        main.cpp
        Thread.cpp
        Thread.h
        ThreadManager.cpp
        ThreadManager.h)
