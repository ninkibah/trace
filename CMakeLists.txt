cmake_minimum_required(VERSION 3.9)
project(trace)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-Wall)


add_executable(trace main.cpp trace.h)