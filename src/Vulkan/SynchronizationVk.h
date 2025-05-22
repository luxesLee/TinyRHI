#pragma once
#include "ISynchronization.h"
#include <vulkan/vulkan.hpp>

namespace TinyRHI
{

    class FenceVk : public IFence
    {
    public:
    

    private:
        vk::UniqueFence fence;
    };

} // namespace TinyRHI
