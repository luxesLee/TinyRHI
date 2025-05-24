#pragma once
#include "HeaderVk.h"

namespace TinyRHI
{

    class CommandPoolManager
    {
    public:
        CommandPoolManager(
            const DeviceData& _deviceData)
            : deviceData(_deviceData)
        {
            auto commandPoolCreateInfo = vk::CommandPoolCreateInfo()
                .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                .setQueueFamilyIndex(deviceData.queueFamilyIndices.graphicsFamilyIndex);
            commandPool = deviceData.logicalDevice.createCommandPoolUnique(commandPoolCreateInfo);

            auto allocInfo = vk::CommandBufferAllocateInfo()
                .setCommandPool(commandPool.get())
                .setCommandBufferCount(1);
            cmdBufferTmp = std::move(deviceData.logicalDevice.allocateCommandBuffersUnique(allocInfo)[0]);
        }

        void SubmitCmdBuffer(
            // const std::vector<vk::Semaphore>& waitSemaphores,
            // const std::vector<vk::Semaphore>& signalSemaphores,
            // const std::optional<vk::PipelineStageFlags>& waitStages,
            const std::optional<vk::Fence> fence)
        {
            vk::SubmitInfo submitInfo = vk::SubmitInfo()
                .setCommandBufferCount(1)
                .setPCommandBuffers(&(cmdBufferTmp.get()))
                // .setWaitSemaphoreCount(waitSemaphores.size())
                // .setPWaitSemaphores(waitSemaphores.data())
                // .setSignalSemaphoreCount(signalSemaphores.size())
                // .setPSignalSemaphores(signalSemaphores.data())
                ;

            // if(waitStages.has_value())
            // {
            //     submitInfo.setPWaitDstStageMask(&waitStages.value());
            // }

            deviceData.graphicsQueue.submit(submitInfo, fence.has_value() ? fence.value() : VK_NULL_HANDLE);

            // activeBuffers
        }

        vk::CommandBuffer GetCmdBuffer()
        {
            return cmdBufferTmp.get();
        }

        auto& CmdPoolHandle()
        {
            return commandPool.get();
        }

    private:
        const DeviceData& deviceData;
        vk::UniqueCommandPool commandPool;
        std::vector<vk::UniqueCommandBuffer> cmdBuffers;
        std::vector<vk::CommandBuffer> activeBuffers;
        vk::UniqueCommandBuffer cmdBufferTmp;
    };

} // namespace TinyRHI
