#include <iostream>
#include "HandleOgl.h"
#include "ShaderOgl.h"
#include <GLFW/glfw3.h>

using namespace TinyRHI;

void TinyRHI::HandleOgl::InitOpenGL()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
    {
        std::cout << "glad load failed" << std::endl;
    }
}

IShader* HandleOgl::CreateVertexShader(const ShaderDesc &shaderDesc)
{
    return new ShaderOgl<IShader::Stage::Vertex>(shaderDesc);
}

IShader* HandleOgl::CreatePixelShader(const ShaderDesc &shaderDesc)
{
    return new ShaderOgl<IShader::Stage::Pixel>(shaderDesc);
}

IShader* HandleOgl::CreateComputeShader(const ShaderDesc &shaderDesc)
{
    return new ShaderOgl<IShader::Stage::Compute>(shaderDesc);
}

IRHIHandle* TinyRHI::HandleOgl::DrawPrimitive(Uint32 baseVertexIndex, Uint32 numPrimitives, Uint32 numInstances)
{
    return this;
}

IRHIHandle* TinyRHI::HandleOgl::DrawPrimitiveIndirect(IBuffer *argumentBuffer, Uint32 argumentOffset)
{
    return this;
}

IRHIHandle* TinyRHI::HandleOgl::DrawIndexPrimitive(IBuffer *indexBuffer, Int32 baseVertexIndex, Uint32 firstInstance, Uint32 startIndex, Uint32 numPrimitives, Uint32 numInstances)
{
    return this;
}

IRHIHandle* HandleOgl::Dispatch(Uint32 threadGroupCountX, Uint32 threadGroupCountY, Uint32 threadGroupCountZ)
{
    glDispatchCompute(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
    return this;
}

IRHIHandle* HandleOgl::SetSamplerTexture(ITexture *texture, Uint setId, Uint bindingId)
{
    return this;
}

IRHIHandle* HandleOgl::SetStorageTexture(ITexture *texture, Uint setId, Uint bindingId)
{
    return this;
}

IRHIHandle* HandleOgl::SetStorageBuffer(IBuffer *buffer, Uint setId, Uint bindingId)
{
    return this;
}

IRHIHandle* HandleOgl::SetUniformBuffer(IBuffer *Buffer, Uint setId, Uint bindingId)
{
    return this;
}
