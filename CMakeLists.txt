cmake_minimum_required(VERSION 2.8.4)

project(flock_herd_school)

SET(${PROJECT_NAME}_MAJOR_VERSION 0)
SET(${PROJECT_NAME}_MINOR_VERSION 1)

find_package(OpenGL REQUIRED)

find_package(glfw REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(HEADERS vector3.h boids.h)
set(SOURCE_FILES main.cpp boids.cpp vector3.cpp)

add_executable(flock_herd_school ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(flock_herd_school glfw3 ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARY})


