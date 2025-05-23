
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)
message("CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

find_program(GLSLANG_VALIDATOR glslangValidator)
message("GLSLANG_VALIDATOR_DIR: ${GLSLANG_VALIDATOR}")

set(SHADER_DIR ${CMAKE_SOURCE_DIR}/test/shader)
message("SHADER_DIR: ${SHADER_DIR}")
set(SPIRV_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shader/spirv)
message("SPIRV_DIR: ${SPIRV_DIR}")

if(GLSLANG_VALIDATOR)
    file(GLOB GLSL_SOURCES "${SHADER_DIR}/*.vert" "${SHADER_DIR}/*.frag")

    # foreach(file ${GLSL_SOURCES})
    #     message("Found shader: ${file}")
    # endforeach()

    # 为每个 GLSL 文件创建一个 custom command 编译为 SPIR-V
    foreach(GLSL_FILE ${GLSL_SOURCES})
        # 获取文件名（不包括路径和扩展名）
        get_filename_component(FILE_NAME ${GLSL_FILE} NAME_WE)
        set(SPIRV_FILE "${SPIRV_DIR}/${FILE_NAME}.spv")

        # 添加自定义命令，使用 glslangValidator 编译 GLSL 文件为 SPIR-V
        add_custom_command(
            OUTPUT ${SPIRV_FILE}
            COMMAND ${GLSLANG_VALIDATOR} -V ${GLSL_FILE} -o ${SPIRV_FILE}
            DEPENDS ${GLSL_FILE}
            COMMENT "Compiling GLSL shader: ${GLSL_FILE}"
        )

        # 将生成的 SPIR-V 文件添加到构建目标
        list(APPEND SPIRV_FILES ${SPIRV_FILE})
    endforeach()

    # 创建一个 custom target，确保着色器文件被编译
    add_custom_target(test_min_example_shader ALL DEPENDS ${SPIRV_FILES})
else()
    message(FATAL_ERROR "glslangValidator not found!")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TinyRHI-min-example TinyRHI_min_example.cpp)
add_dependencies(TinyRHI-min-example test_min_example_shader)

target_link_libraries(TinyRHI-min-example PRIVATE TinyRHI)

set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
target_link_libraries(TinyRHI-min-example PUBLIC glfw)

add_test(NAME TinyRHITest COMMAND TinyRHI-min-example)