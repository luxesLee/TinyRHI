#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Test.h"

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1024, 1024, "Test_Mini_Example", nullptr, nullptr);
    TinyRHI::IRHIHandle* pHandle = TinyRHI::RHIFactory::getHandle(window);

    


    // while(!glfwWindowShouldClose(window))
    {
        // pHandle->BeginFrame();
        //     pHandle->BeginCommand();
        //         pHandle->BeginRenderPass();

        //         pHandle->EndRenderPass();
        //     pHandle->EndCommand();

        //     pHandle->Commit();
        // pHandle->EndFrame();

    }

    glfwDestroyWindow(window);

    return 0;
}
