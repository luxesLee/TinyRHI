#include "OpenGLHandle.h"
#include <GLFW/glfw3.h>
#include "glad/glad.h"
#include <iostream>

#include "OglShader.h"

using namespace TinyRHI;

OpenGLHandle::OpenGLHandle()
{
    // Init Ogl Environment
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
    {
        std::cout << "glad load failed" << std::endl;
    }
}

OpenGLHandle::~OpenGLHandle()
{
}

IShader* OpenGLHandle::CreateVertexShader(ShaderDesc &shaderDesc)
{
    return new OglShader(shaderDesc, IShader::Stage::Vertex);
}

IShader* OpenGLHandle::CreatePixelShader(ShaderDesc &shaderDesc)
{
    return new OglShader(shaderDesc, IShader::Stage::Pixel);
}

IShader* OpenGLHandle::CreateComputeShader(ShaderDesc &shaderDesc)
{
    return new OglShader(shaderDesc, IShader::Stage::Compute);
}

void TinyRHI::OpenGLHandle::DrawPrimitive(Uint32 baseVertexIndex, Uint32 numPrimitives, Uint32 numInstances)
{
    
}

void TinyRHI::OpenGLHandle::DrawPrimitiveIndirect(IBuffer *argumentBuffer, Uint32 argumentOffset)
{

}

void TinyRHI::OpenGLHandle::DrawIndexPrimitive(IBuffer *indexBuffer, Int32 baseVertexIndex, Uint32 firstInstance, Uint32 startIndex, Uint32 numPrimitives, Uint32 numInstances)
{

}

void OpenGLHandle::Dispatch(Uint32 threadGroupCountX, Uint32 threadGroupCountY, Uint32 threadGroupCountZ)
{
    glDispatchCompute(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}
