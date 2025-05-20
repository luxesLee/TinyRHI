#pragma once
#include "glad/glad.h"
#include "IFramebuffer.h"

namespace TinyRHI
{
    class OglFramebuffer : public IFramebuffer
    {
    


    private:
        GLuint framebufferId;
    };

} // namespace TinyRHI