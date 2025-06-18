#pragma once
#include "glad/glad.h"
#include "IFramebuffer.h"

namespace TinyRHI
{
    class FramebufferOgl : public IFramebuffer
    {
    public:
        FramebufferOgl()
        {
            
        }

    private:
        GLuint framebufferId;
    };

} // namespace TinyRHI