#ifdef RHI_SUPPORT_OPENGL

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

IRHIHandle* HandleOgl::SetViewport(Extent2D minExt, Extent2D maxExt)
{
    glViewport(minExt.width, minExt.height, maxExt.width - minExt.width, maxExt.height - minExt.height);
    return this;
}

IRHIHandle* HandleOgl::SetViewport(Extent3D minExt, Extent3D maxExt)
{
    glViewport(minExt.width, minExt.height, maxExt.width - minExt.width, maxExt.height - minExt.height);
    return this;
}

IRHIHandle* HandleOgl::SetScissor(Extent2D minExt, Extent2D maxExt)
{
    

    glScissor(minExt.width, minExt.height, maxExt.width - minExt.width, maxExt.height - minExt.height);
    return this;
}

#endif
