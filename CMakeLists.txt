cmake_minimum_required(VERSION 3.15.0)
project(TinyRHI VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)

set(GLFW_DIR ${CMAKE_SOURCE_DIR}/third_party/glfw)
include_directories(${GLFW_DIR}/include)

add_library(TinyRHI STATIC TinyRHI.cpp)

set_target_properties(TinyRHI PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)