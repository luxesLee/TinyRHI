#pragma once
#ifdef RHI_SUPPORT_OPENGL
#include "glad/glad.h"
#include "IBuffer.h"

namespace TinyRHI
{
    class BufferOgl : public IBuffer
    {
    public:
        BufferOgl()
        {
            
        }


    private:
        GLuint bufferId;
    };

} // namespace TinyRHI

#endif
