#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>
#include <random>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "RHIHandleFactory.h"
#include "IBuffer.h"

const uint32_t PARTICLE_COUNT = 8192;

struct Particle 
{
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec4 color;
};

struct UniformBufferObject 
{
    float deltaTime = 1.0f;
} ubo;

static std::vector<char> readFile(const std::string& filename) 
{
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

void ProduceParticle(std::vector<Particle>& particles)
{
    // Initialize particles
    std::default_random_engine rndEngine((unsigned)time(nullptr));
    std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

    // Initial particle positions on a circle

    for (auto& particle : particles) {
        float r = 0.25f * sqrt(rndDist(rndEngine));
        float theta = rndDist(rndEngine) * 2.0f * 3.14159265358979323846f;
        float x = r * cos(theta);
        float y = r * sin(theta);
        particle.position = glm::vec2(x, y);
        particle.velocity = glm::normalize(glm::vec2(x,y)) * 0.00025f;
        particle.color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
    }
}

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

    std::vector<Particle> particles(PARTICLE_COUNT);
    ProduceParticle(particles);
    TinyRHI::IBuffer* pParticleStorageBuffer[2];
    pParticleStorageBuffer[0] = pHandle->CreateBufferWithData(
        TinyRHI::BufferDesc
        {
            .bufferType
            {
                .bVertex = true,
                .bStorage = true,
            },
            .elementNum = PARTICLE_COUNT,
            .stride = sizeof(Particle),
            .bStaging = true,
        }, particles.data(), sizeof(Particle) * PARTICLE_COUNT);

    pParticleStorageBuffer[1] = pHandle->CreateBuffer(
        TinyRHI::BufferDesc
        {
            .bufferType
            {
                .bVertex = true,
                .bStorage = true,
            },
            .elementNum = PARTICLE_COUNT,
            .stride = sizeof(Particle),
            .bStaging = true,
        });

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

    auto vertShaderCode = readFile("shader/spirv/test_computeShader_example_vert.spv");
    auto vertShader = pHandle->CreateVertexShader(TinyRHI::ShaderDesc
    {
        .codeData = vertShaderCode.data(),
        .codeSize = (Uint32)vertShaderCode.size(),
    });

    auto pixelShaderCode = readFile("shader/spirv/test_computeShader_example_frag.spv");
    auto pixelShader = pHandle->CreatePixelShader(TinyRHI::ShaderDesc
    {
        .codeData = pixelShaderCode.data(),
        .codeSize = (Uint32)pixelShaderCode.size(),
    });

    auto compShaderCode = readFile("shader/spirv/test_computeShader_example_comp.spv");
    auto compShader = pHandle->CreateComputeShader(TinyRHI::ShaderDesc
    {
        .codeData = compShaderCode.data(),
        .codeSize = (Uint32)compShaderCode.size(),
    });


    auto gfxSetting = TinyRHI::GfxSetting
    {
        .vertexDecl
        {
            .vertexBindings
            {
                {
                    .binding = 0,
                    .stride = sizeof(Particle),
                    .bInstance = false,
                },
            },
            .attributeDescs
            {
                TinyRHI::VertexDeclaration::VertexAttributeDesc
                {
                    .location = 0,
                    .binding = 0,
                    .offset = offsetof(Particle, Particle::position),
                    .format = TinyRHI::AttribType::Vec2,
                },
                TinyRHI::VertexDeclaration::VertexAttributeDesc
                {
                    .location = 1,
                    .binding = 0,
                    .offset = offsetof(Particle, Particle::color),
                    .format = TinyRHI::AttribType::Vec4,
                },
            },
        },
        .blendSettings
        {
            TinyRHI::BlendSetting::Opaque,
        },
    };

    double currentTime, lastTime = glfwGetTime();
    Uint curInputStorageBufferIndex = 0;
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        currentTime = glfwGetTime();
        lastTime = (currentTime - lastTime) * 1000.0;
        ubo.deltaTime = lastTime * 2.0f;
        lastTime = currentTime;

        pHandle->
            BeginFrame()->
                BeginCommand()->
                    // Pass1: CompShader update Particle
                    UpdateBuffer(pUniformBuffer, &ubo, sizeof(ubo), 0)->
                        SetComputeShader(compShader)->
                        SetUniformBuffer(pUniformBuffer, TinyRHI::IShader::Stage::Compute, 0, 0)->
                        SetStorageBuffer(pParticleStorageBuffer[curInputStorageBufferIndex], TinyRHI::IShader::Stage::Compute, 0, 1)->
                        SetStorageBuffer(pParticleStorageBuffer[(curInputStorageBufferIndex + 1) % 2], TinyRHI::IShader::Stage::Compute, 0, 2)->
                        SetComputePipeline()->
                        Dispatch(PARTICLE_COUNT / 256, 1, 1)->
                EndCommand()->
                Commit()->
                BeginCommand()->
                    // Pass2: GfxShader present Particle
                    SetDefaultAttachments(attachmentDesc)->
                    BeginRenderPass()->
                        SetVertexShader(vertShader)->
                        SetPixelShader(pixelShader)->
                        SetGraphicsPipeline(gfxSetting)->
                        SetViewport(Extent2D(0, 0), Extent2D(1024, 1024))->
                        SetScissor(Extent2D(0, 0), Extent2D(1024, 1024))->
                        SetVertexStream(0, pParticleStorageBuffer[curInputStorageBufferIndex], 0)->
                        DrawPrimitive(PARTICLE_COUNT, 0)->
                    EndRenderPass()->
                EndCommand()->
                Commit()->
            EndFrame();

        curInputStorageBufferIndex = (curInputStorageBufferIndex + 1) % 2;
    }

    glfwDestroyWindow(window);

    return 0;
}