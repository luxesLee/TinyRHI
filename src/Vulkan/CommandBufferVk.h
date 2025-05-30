#pragma once
#include "HeaderVk.h"
#include "UniqueHash.h"

namespace TinyRHI
{
    class CommandBufferVk : public UniqueHash
    {
    public:
        CommandBufferVk(const DeviceData& _deviceData, vk::UniqueCommandBuffer _cmdBuffer)
            : deviceData(_deviceData), cmdBuffer(std::move(_cmdBuffer))
        {
            auto fenceCreateInfo = vk::FenceCreateInfo()
                .setFlags(vk::FenceCreateFlagBits::eSignaled);
            cmdFence = deviceData.logicalDevice.createFenceUnique(fenceCreateInfo);
            Reset();
        }

        ~CommandBufferVk()
        {

        }

        void BeginCommand()
        {
            cmdBuffer->reset();
            cmdBuffer->begin(vk::CommandBufferBeginInfo());
        }

        void EndCommand()
        {
            cmdBuffer->end();
        }

        void Submit(vk::Queue queue)
        {
            vk::SubmitInfo submitInfo = vk::SubmitInfo()
                .setCommandBufferCount(1)
                .setPCommandBuffers(&(cmdBuffer.get()))
                .setWaitSemaphoreCount(cmdWaitSemaphores.size())
                .setPWaitSemaphores(cmdWaitSemaphores.data())
                .setSignalSemaphoreCount(cmdSignalSemaphores.size())
                .setPSignalSemaphores(cmdSignalSemaphores.data())
                ;

            if(waitStages.size() > 0)
            {
                submitInfo.setPWaitDstStageMask(waitStages.data());
            }

            queue.submit(submitInfo, cmdFence.get());
        }

        Bool QueryComplete()
        {
            return vk::Result::eSuccess == deviceData.logicalDevice.getFenceStatus(cmdFence.get());
        }

        void Reset()
        {
            deviceData.logicalDevice.resetFences(cmdFence.get());
            // todo: pool to manage
            cmdWaitSemaphores.clear();
            cmdSignalSemaphores.clear();
            waitStages.clear();
        }

        void AddWaitSemaphore(vk::Semaphore semaphore)
        {
            cmdWaitSemaphores.push_back(semaphore);
        }

        void AddSignalSemaphore(vk::Semaphore semaphore)
        {
            cmdSignalSemaphores.push_back(semaphore);
        }

        void AddWaitStage(vk::PipelineStageFlags flags)
        {
            waitStages.push_back(flags);
        }

        vk::CommandBuffer Get()
        {
            return cmdBuffer.get();
        }

    private:
        const DeviceData& deviceData;

        vk::UniqueCommandBuffer cmdBuffer;
        // Ensure availability when acquiring the queue
        vk::UniqueFence cmdFence;
        std::vector<vk::Semaphore> cmdWaitSemaphores;
        std::vector<vk::Semaphore> cmdSignalSemaphores;
        std::vector<vk::PipelineStageFlags> waitStages;
    };

} // namespace TinyRHI
