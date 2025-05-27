#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Test.h"
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
    -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
};

std::vector<Uint16> indices = 
{
    0, 1, 2, 2, 3, 0
};

struct UniformBufferObject 
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
} ubo;

void updateUniformBuffer()
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1024, 1024, "Test_ImageSampler_Example", nullptr, nullptr);
    TinyRHI::IRHIHandle* pHandle = TinyRHI::RHIFactory::getHandle(window);

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
            .elementNum = 4,
            .stride = sizeof(Float) * 7,
            .bStaging = true,
        }, vertex.data(), sizeof(Float) * vertex.size());

    TinyRHI::IBuffer* pIndex = pHandle->CreateBufferWithData(
        TinyRHI::BufferDesc
        {
            .bufferType
            {
                .bIndex = true
            },
            .elementNum = 6,
            .stride = sizeof(Uint16),
            .bStaging = true,
        }, indices.data(), sizeof(Uint16) * indices.size());

    TinyRHI::IBuffer* pUniformBuffer = pHandle->CreateBuffer(
        TinyRHI::BufferDesc
        {
            .bufferType
            {
                .bUniform = true
            },
            .elementNum = 1,
            .stride = sizeof(UniformBufferObject),
            .bStaging = false,
        });

    Int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("res/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    TinyRHI::ITexture* pSamplerTex = pHandle->CreateTextureWithData(
        TinyRHI::ImageDesc
        {
            .size3 = {(Uint)texWidth, (Uint)texHeight, 1},
            .format = Format::RGBA8_SRGB,
            .bStaging = true,
            .usage = 
            {
                .Sample = true,
            },
        },
        TinyRHI::SamplerState
        {
            .anisotropyEnable = false,
            .compOp = CompOp::Always,
            .filterType = TinyRHI::SamplerState::FilterType::Linear,
            .samplerMipmap = TinyRHI::SamplerState::FilterType::Linear,
        }, pixels, texWidth * texHeight * 4);
    stbi_image_free(pixels);

    auto vertShaderCode = readFile("shader/spirv/test_imageSampler_example_vert.spv");
    auto vertShader = pHandle->CreateVertexShader(TinyRHI::ShaderDesc
    {
        .codeData = vertShaderCode.data(),
        .codeSize = (Uint32)vertShaderCode.size(),
    });

    auto pixelShaderCode = readFile("shader/spirv/test_imageSampler_example_frag.spv");
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
                    .stride = sizeof(Float) * 7,
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
                TinyRHI::VertexDeclaration::VertexAttributeDesc
                {
                    .location = 2,
                    .binding = 0,
                    .offset = 5 * sizeof(Float),
                    .format = TinyRHI::AttribType::Vec2,
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

        updateUniformBuffer();

        pHandle->
            BeginFrame()->
                BeginCommand()->
                    SetDefaultAttachments(attachmentDesc)->
                    UpdateBuffer(pUniformBuffer, &ubo, sizeof(ubo), 0)->
                    BeginRenderPass()->
                        SetVertexShader(vertShader)->
                        SetPixelShader(pixelShader)->
			            SetUniformBuffer(pUniformBuffer, TinyRHI::IShader::Stage::Vertex, 0, 0)->
			            SetSamplerTexture(pSamplerTex, TinyRHI::IShader::Stage::Pixel, 0, 1)->
                        SetGraphicsPipeline(gfxSetting)->
                        SetViewport(Extent2D(0, 0), Extent2D(1024, 1024))->
                        SetScissor(Extent2D(0, 0), Extent2D(1024, 1024))->
                        SetVertexStream(0, pVeretx, 0)->
                        DrawIndexPrimitive(pIndex, 6, 0, 0)->
                    EndRenderPass()->
                EndCommand()->
                Commit()->
            EndFrame();
    }

    glfwDestroyWindow(window);

    return 0;
}