#include "Test.h"
#include "HandleVk.h"
#include "GLFW/glfw3.h"

namespace TinyRHI
{
    IRHIHandle *RHIFactory::getHandle(GLFWwindow *window)
    {
        return new VkHandle(window);
    }

} // namespace TinyRHI
