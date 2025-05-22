#include "Test.h"
#include "HandleVk.h"

namespace TinyRHI
{
    IRHIHandle *getHandle()
    {
        return new VkHandle();
    }

} // namespace TinyRHI
