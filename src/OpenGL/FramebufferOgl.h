#pragma once
#include "glad/glad.h"
#include "IFramebuffer.h"

namespace TinyRHI
{
    class FramebufferOgl : public IFramebuffer
    {
    


    private:
        GLuint framebufferId;
    };

} // namespace TinyRHI