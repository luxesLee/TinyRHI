# Shader
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)
message("CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

find_program(GLSLANG_VALIDATOR glslangValidator)
message("GLSLANG_VALIDATOR_DIR: ${GLSLANG_VALIDATOR}")

set(SHADER_DIR ${CMAKE_SOURCE_DIR}/test/shader)
message("SHADER_DIR: ${SHADER_DIR}")
set(SPIRV_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shader/spirv)
message("SPIRV_DIR: ${SPIRV_DIR}")

if(GLSLANG_VALIDATOR)
    file(GLOB GLSL_SOURCES "${SHADER_DIR}/*.vert" "${SHADER_DIR}/*.frag" "${SHADER_DIR}/*.comp")

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
    add_custom_target(test_example_shader ALL DEPENDS ${SPIRV_FILES})
else()
    message(FATAL_ERROR "glslangValidator not found!")
endif()

# Texture
set(TEXTURE_SRC_DIRECTORY ${CMAKE_SOURCE_DIR}/res)
set(TEXTURES_DST_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY ${TEXTURE_SRC_DIRECTORY} DESTINATION ${TEXTURES_DST_DIRECTORY})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF)
set(TINYGLTF_INSTALL OFF)
set(TINYGLTF_INSTALL_VENDOR OFF)

set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)

add_executable(TinyRHI-min-example TinyRHI_min_example.cpp)
add_dependencies(TinyRHI-min-example test_example_shader)
target_link_libraries(TinyRHI-min-example PRIVATE TinyRHI)
target_link_libraries(TinyRHI-min-example PUBLIC glfw)
add_test(NAME TinyRHITest COMMAND TinyRHI-min-example)

add_executable(TinyRHI-ImageSampler-example TinyRHI_imageSampler_example.cpp)
add_dependencies(TinyRHI-ImageSampler-example test_example_shader)
target_link_libraries(TinyRHI-ImageSampler-example PRIVATE TinyRHI)
target_link_libraries(TinyRHI-ImageSampler-example PUBLIC glfw)
target_link_libraries(TinyRHI-ImageSampler-example PUBLIC tinygltf)
target_link_libraries(TinyRHI-ImageSampler-example PUBLIC glm)
add_test(NAME TinyRHITest2 COMMAND TinyRHI-ImageSampler-example)

add_executable(TinyRHI-CompShader-example TinyRHI_computeShader_example.cpp)
add_dependencies(TinyRHI-CompShader-example test_example_shader)
target_link_libraries(TinyRHI-CompShader-example PRIVATE TinyRHI)
target_link_libraries(TinyRHI-CompShader-example PUBLIC glfw)
target_link_libraries(TinyRHI-CompShader-example PUBLIC tinygltf)
target_link_libraries(TinyRHI-CompShader-example PUBLIC glm)
add_test(NAME TinyRHITest3 COMMAND TinyRHI-CompShader-example)