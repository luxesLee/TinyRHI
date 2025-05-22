#define USE_VULKAN 1

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include "Test.h"

int main()
{
    TinyRHI::IRHIHandle* pHandle = TinyRHI::getHandle();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1024, 1024, "Test_Mini_Example", nullptr, nullptr);

    

    // while(!glfwWindowShouldClose(window))
    {



    }

    glfwDestroyWindow(window);

    return 0;
}
