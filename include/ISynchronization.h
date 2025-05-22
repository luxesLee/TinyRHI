#pragma once

namespace TinyRHI
{
    class IFence
    {
    public:
        virtual void Wait() = 0;
        virtual void Signal() = 0;
        virtual void Reset() = 0;
    };

    class ISemaphore
    {

    };

} // namespace TinyRHI
