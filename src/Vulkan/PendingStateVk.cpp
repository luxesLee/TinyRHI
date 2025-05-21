#include "PendingStateVk.h"

using namespace TinyRHI;

void GfxPendingStateVk::PrepareDraw()
{

    UpdateDynamicStates();

    if(bDescriptorSetDirty)
    {
        // 1. write change to descriptorSet

        // 2. bind descriptorSet

    }
}

void ComputePendingStateVk::PrepareDispatch()
{

    if(bDescriptorSetDirty)
    {
        // 1. write change to descriptorSet

        // 2. bind descriptorSet
        // vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0, 1, &computeDescriptorSets[currentFrame], 0, nullptr);
    }
}
