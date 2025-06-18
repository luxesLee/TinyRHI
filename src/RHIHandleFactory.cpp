#include "RHIHandleFactory.h"
#include "GLFW/glfw3.h"

#ifdef RHI_SUPPORT_VULKAN
#include "Vulkan/HandleVk.h"
#elif RHI_SUPPORT_OPENGL
#include "OpenGL/HandleOgl.h"
#else

#endif


namespace TinyRHI
{
    IRHIHandle* RHIHandleFactory::getHandle(GLFWwindow *window)
    {
        #ifdef RHI_SUPPORT_VULKAN
        return new VkHandle(window);
        #elif RHI_SUPPORT_OPENGL
        return new HandleOgl(window);
        #else
        return nullptr;
        #endif
    }

} // namespace TinyRHI
