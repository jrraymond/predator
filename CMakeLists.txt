cmake_minimum_required(VERSION 2.8.4)
project(flock_herd_school)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(HEADERS vector3.h boids.h)
set(SOURCE_FILES main.cpp boids.cpp vector3.cpp)
add_executable(flock_herd_school ${SOURCE_FILES})

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(flock_herd_school ${SDL2_LIBRARIES})