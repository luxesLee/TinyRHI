#pragma once
#ifdef RHI_SUPPORT_VULKAN

#include "HeaderVk.h"
#include "CommandBufferVk.h"
#include <queue>
#include <unordered_map>

namespace TinyRHI
{

#define MAX_COMMANDS_NUM 20
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
                .setCommandBufferCount(MAX_COMMANDS_NUM)
                .setCommandPool(commandPool.get());
            auto cmdBufferTmpArray = deviceData.logicalDevice.allocateCommandBuffersUnique(allocInfo);
            for(Uint i = 0; i < MAX_COMMANDS_NUM; i++)
            {
                idleCmdBuffersQueue.push(new CommandBufferVk(deviceData, std::move(cmdBufferTmpArray[i])));
            }
        }

        CommandBufferVk* GetCmdBuffer()
        {
            auto GetIdleCmdBuffer = [&]() ->CommandBufferVk*
            {
                if(idleCmdBuffersQueue.size() > 0)
                {
                    auto cmdBuffer = idleCmdBuffersQueue.front();
                    idleCmdBuffersQueue.pop();
                    activeCmdBuffersSet[cmdBuffer->Hash()] = cmdBuffer;
                    return cmdBuffer;
                }
                return nullptr;
            };

            if(CommandBufferVk* cmdBuffer = GetIdleCmdBuffer())
            {
                return cmdBuffer;
            }

            if(submitCmdBuffersSet.size() > 0)
            {
                for (auto it = submitCmdBuffersSet.begin(); it != submitCmdBuffersSet.end();)
                {
                    auto& [key, val] = *it;
                    if (val && val->QueryComplete())
                    {
                        val->Reset();
                        idleCmdBuffersQueue.push(val);
                        it = submitCmdBuffersSet.erase(it);
                    }
                    else
                    {
                        it++;
                    }
                }
            }

            if(CommandBufferVk* cmdBuffer = GetIdleCmdBuffer())
            {
                return cmdBuffer;
            }
            
            auto allocInfo = vk::CommandBufferAllocateInfo()
                .setCommandBufferCount(MAX_COMMANDS_NUM)
                .setCommandPool(commandPool.get());
            auto cmdBufferTmpArray = deviceData.logicalDevice.allocateCommandBuffersUnique(allocInfo);
            for(Uint i = 0; i < MAX_COMMANDS_NUM; i++)
            {
                idleCmdBuffersQueue.push(new CommandBufferVk(deviceData, std::move(cmdBufferTmpArray[i])));
            }

            return GetIdleCmdBuffer();
        }

        void SubmitCmdBuffer(CommandBufferVk* cmdBuffer, vk::Queue queue)
        {
            assert(activeCmdBuffersSet.contains(cmdBuffer->Hash()));

            activeCmdBuffersSet.erase(cmdBuffer->Hash());
            submitCmdBuffersSet[cmdBuffer->Hash()] = cmdBuffer;

            cmdBuffer->Submit(queue);
        }

        auto& CmdPoolHandle()
        {
            return commandPool.get();
        }

    private:
        const DeviceData& deviceData;
        vk::UniqueCommandPool commandPool;

        std::queue<CommandBufferVk*> idleCmdBuffersQueue;
        std::unordered_map<Uint32, CommandBufferVk*> activeCmdBuffersSet;
        std::unordered_map<Uint32, CommandBufferVk*> submitCmdBuffersSet;
    };

} // namespace TinyRHI

#endif
