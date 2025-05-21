#pragma once
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
