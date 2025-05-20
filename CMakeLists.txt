cmake_minimum_required(VERSION 3.15.0)
project(TinyRHI VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)

set(GLAD_DIR ${CMAKE_SOURCE_DIR}/third_party/glad)
set(GLFW_DIR ${CMAKE_SOURCE_DIR}/third_party/glfw)
set(GLM_DIR ${CMAKE_SOURCE_DIR}/third_party/glm)

file(GLOB HEADER_FILES "${PROJECT_SOURCE_DIR}/src/**/*.h")
# foreach(file ${HEADER_FILES})
#     message(STATUS "Found header: ${file}")
# endforeach()

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${HEADER_FILES}
    ${GLAD_DIR}/include
    ${GLFW_DIR}/include
    ${GLM_DIR}/include)

file(GLOB SRC_FILES "${PROJECT_SOURCE_DIR}/src/**/*.cpp")
foreach(file ${SRC_FILES})
    message(STATUS "Found header: ${file}")
endforeach()

add_library(TinyRHI STATIC ${SRC_FILES})

set_target_properties(TinyRHI PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)