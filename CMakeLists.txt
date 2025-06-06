cmake_minimum_required(VERSION 3.15.0)
project(TinyRHI VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TEST_EXAMPLE ON)

if(MSVC)
    set(CMAKE_C_FLAGS /source-charset:utf-8)
endif()

find_package(Vulkan REQUIRED)
message(STATUS "Vulkan_INCLUDE_DIRS: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Vulkan_LIBRARIES: ${Vulkan_LIBRARIES}")

find_package(OpenGL REQUIRED)

set(GLAD_DIR ${CMAKE_SOURCE_DIR}/third_party/glad)
set(GLFW_DIR ${CMAKE_SOURCE_DIR}/third_party/glfw)
set(GLM_DIR ${CMAKE_SOURCE_DIR}/third_party/glm)

# file(GLOB HEADER_FILES "${PROJECT_SOURCE_DIR}/src/**/*.h")
# foreach(file ${HEADER_FILES})
#     message(STATUS "Found header: ${file}")
# endforeach()

include_directories(
    ${GLAD_DIR}/include
    ${GLFW_DIR}/include
    ${GLM_DIR})

file(GLOB SRC_FILES "${PROJECT_SOURCE_DIR}/src/**/*.cpp")
foreach(file ${SRC_FILES})
    message(STATUS "Found header: ${file}")
endforeach()

add_library(TinyRHI STATIC ${SRC_FILES})

if(WIN32)
    target_compile_definitions(TinyRHI PRIVATE WIN_MACRO)
elseif(UNIX)
    target_compile_definitions(TinyRHI PRIVATE LINUX_MACRO)
endif()

target_compile_definitions(TinyRHI PRIVATE DEBUG_VULKAN_MACRO)

target_include_directories(TinyRHI PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(TinyRHI PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(TinyRHI ${Vulkan_LIBRARIES})

set_target_properties(TinyRHI PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

# Test
if(TEST_EXAMPLE)
    add_subdirectory(third_party/glfw)
    add_subdirectory(third_party/tinygltf)
    add_subdirectory(third_party/glm)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()
