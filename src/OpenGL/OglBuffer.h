#pragma once
#include "glad/glad.h"
#include "IBuffer.h"

namespace TinyRHI
{
    class OglBuffer : public IBuffer
    {
    public:
        OglBuffer()
        {
            
        }


    private:
        GLuint bufferId;
    };

} // namespace TinyRHI
