#pragma once
#include "IRHIHandle.h"

struct GLFWwindow;

namespace TinyRHI
{    
    class RHIHandleFactory
    {
    public:
        static IRHIHandle* getHandle(GLFWwindow* window);
    };
    
} // namespace TinyRHI
