cmake_minimum_required(VERSION 2.8.4)
project(flock_herd_school)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(flock_herd_school ${SOURCE_FILES})