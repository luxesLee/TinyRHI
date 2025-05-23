#pragma once
#include "IRHIHandle.h"

struct GLFWwindow;

namespace TinyRHI
{    
    class RHIFactory
    {
    public:
        static IRHIHandle* getHandle(GLFWwindow* window);
    };
    
} // namespace TinyRHI
