cmake_minimum_required(VERSION 2.8.4)

project(flock_herd_school)

SET(${PROJECT_NAME}_MAJOR_VERSION 0)
SET(${PROJECT_NAME}_MINOR_VERSION 1)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})

#find_package(glfw REQUIRED)
#include_directories(${GLFW_INCLUDE_DIRS})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

set(FREETYPE_INCLUDE_DIRS "/usr/include/freetype2/")
set(FREETYPE_LIBRARY "/usr/lib/x86_64-linux-gnu/libfreetype.so.6")
find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(HEADERS vector3.h boids.h shader_utils.h object.h text.h)
set(SOURCE_FILES main.cpp boids.cpp vector3.cpp shader_utils.cpp object.cpp text.cpp)
add_executable(flock_herd_school ${SOURCE_FILES})

#target_link_libraries(flock_herd_school ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
target_link_libraries(flock_herd_school  ${GLFW_LIBRARIES} X11 Xxf86vm Xrandr pthread Xi ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${FREETYPE_LIBRARIES})

set(shaders "vertex_shader.glsl"
            "fragment_shader.glsl"
            "text_vertex_shader.glsl"
            "text_fragment_shader.glsl"
            "OpenSans-Regular.ttf"
            "FreeSans.ttf")
if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    foreach(item IN LISTS shaders)
        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/${item}
            ${CMAKE_CURRENT_BINARY_DIR}/${item}
            COPYONLY
        )
    endforeach()
endif()

