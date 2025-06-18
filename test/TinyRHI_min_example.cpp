#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "RHIHandleFactory.h"
#include "IBuffer.h"

static std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

std::vector<Float> vertex = 
{
    0.0f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1024, 1024, "Test_Mini_Example", nullptr, nullptr);
    TinyRHI::IRHIHandle* pHandle = TinyRHI::RHIHandleFactory::getHandle(window);

    TinyRHI::AttachmentDesc attachmentDesc
    {
        .format = Format::BGRA8_SRGB,
        .loadOp = TinyRHI::AttachmentDesc::LoadOp::Clear,
        .clearValue = 
        {
            .color = {0, 0, 0, 1}
        }
    };

    TinyRHI::IBuffer* pVeretx = pHandle->CreateBufferWithData(
        TinyRHI::BufferDesc
        {
            .bufferType
            {
                .bVertex = true
            },
            .elementNum = 3,
            .stride = sizeof(Float) * 5,
            .bStaging = true,
        }, vertex.data(), sizeof(Float) * vertex.size());

    auto vertShaderCode = readFile("shader/spirv/test_min_example_vert.spv");
    auto vertShader = pHandle->CreateVertexShader(TinyRHI::ShaderDesc
    {
        .codeData = vertShaderCode.data(),
        .codeSize = (Uint32)vertShaderCode.size(),
    });

    auto pixelShaderCode = readFile("shader/spirv/test_min_example_frag.spv");
    auto pixelShader = pHandle->CreatePixelShader(TinyRHI::ShaderDesc
    {
        .codeData = pixelShaderCode.data(),
        .codeSize = (Uint32)pixelShaderCode.size(),
    });

    auto gfxSetting = TinyRHI::GfxSetting
    {
        .vertexDecl
        {
            .vertexBindings
            {
                {
                    .binding = 0,
                    .stride = sizeof(Float) * 5,
                    .bInstance = false,
                },
            },
            .attributeDescs
            {
                TinyRHI::VertexDeclaration::VertexAttributeDesc
                {
                    .location = 0,
                    .binding = 0,
                    .offset = 0,
                    .format = TinyRHI::AttribType::Vec2,
                },
                TinyRHI::VertexDeclaration::VertexAttributeDesc
                {
                    .location = 1,
                    .binding = 0,
                    .offset = 2 * sizeof(Float),
                    .format = TinyRHI::AttribType::Vec3,
                },
            },
        },
        .blendSettings
        {
            TinyRHI::BlendSetting::Opaque,
        },
    };

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        pHandle->
            BeginFrame()->
                BeginCommand()->
                    // Attachment need to be set before renderPassBegin to get renderPass and framebuffer
                    SetDefaultAttachments(attachmentDesc)->
                    // SetDepthAttachment()->
                    BeginRenderPass()->
                        SetVertexShader(vertShader)->
                        SetPixelShader(pixelShader)->
                        // Buffer Texture need be set before setpipeline to get pipelineLayout
                        SetGraphicsPipeline(gfxSetting)->
                        SetViewport(Extent2D(0, 0), Extent2D(1024, 1024))->
                        SetScissor(Extent2D(0, 0), Extent2D(1024, 1024))->
                        SetVertexStream(0, pVeretx, 0)->
                        DrawPrimitive(3, 0)->
                    EndRenderPass()->
                EndCommand()->
                Commit()->
            EndFrame();
    }

    glfwDestroyWindow(window);

    return 0;
}
