#ifdef RHI_SUPPORT_VULKAN

#include "BufferVk.h"

using namespace TinyRHI;

BufferVk::BufferVk(
    const DeviceData& _deviceData,
    const BufferDesc& _bufferDesc)
    : deviceData(_deviceData), bufferDesc(_bufferDesc)
{
    this->size = bufferDesc.elementNum * bufferDesc.stride;
    this->mappedDataPtr = nullptr;

    auto usageFlags = ConvertBufferUsage(bufferDesc.bufferType);
    vk::MemoryPropertyFlags memProp;
    if (bufferDesc.bStaging)
    {
        usageFlags |= vk::BufferUsageFlagBits::eTransferDst;
        memProp = vk::MemoryPropertyFlagBits::eDeviceLocal;
    }
    else
    {
        memProp = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    }
    
    auto bufferInfo = vk::BufferCreateInfo()
        .setSize(size)
        .setUsage(usageFlags)
        .setSharingMode(vk::SharingMode::eExclusive);
    buffer = deviceData.logicalDevice.createBufferUnique(bufferInfo);

    vk::MemoryRequirements bufferMemRequirements = deviceData.logicalDevice.getBufferMemoryRequirements(buffer.get());
    vk::PhysicalDeviceMemoryProperties availableMemProperties = deviceData.physicalDevice.getMemoryProperties();
    auto allocInfo = vk::MemoryAllocateInfo()
        .setAllocationSize(bufferMemRequirements.size)
        .setMemoryTypeIndex(findMemoryType(availableMemProperties, memProp, bufferMemRequirements.memoryTypeBits));

    bufferMemory = deviceData.logicalDevice.allocateMemoryUnique(allocInfo);

    deviceData.logicalDevice.bindBufferMemory(buffer.get(), bufferMemory.get(), 0);
}

void BufferVk::SetBufferData(void *data, Uint32 dataSize, Uint32 offset)
{
    if (!bufferDesc.bStaging)
    {
        UpdateBufferData(data, dataSize, offset);
        UnMap();
    }
    else
    {
        auto bufferInfo = vk::BufferCreateInfo()
            .setSize(dataSize)
            .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
            .setSharingMode(vk::SharingMode::eExclusive);
        vk::UniqueBuffer stagingBuffer = deviceData.logicalDevice.createBufferUnique(bufferInfo);
        vk::MemoryRequirements bufferMemRequirements = deviceData.logicalDevice.getBufferMemoryRequirements(buffer.get());
        vk::PhysicalDeviceMemoryProperties availableMemProperties = deviceData.physicalDevice.getMemoryProperties();
        auto allocInfo = vk::MemoryAllocateInfo()
            .setAllocationSize(bufferMemRequirements.size)
            .setMemoryTypeIndex(findMemoryType(
                availableMemProperties, 
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                bufferMemRequirements.memoryTypeBits));
        
        auto stagingMemory = deviceData.logicalDevice.allocateMemoryUnique(allocInfo);
        deviceData.logicalDevice.bindBufferMemory(stagingBuffer.get(), stagingMemory.get(), 0);

        void* stagingPtr = deviceData.logicalDevice.mapMemory(stagingMemory.get(), 0, dataSize);
        memcpy_s(stagingPtr, dataSize, data, dataSize);
        deviceData.logicalDevice.unmapMemory(stagingMemory.get());

        vk::BufferCopy region = vk::BufferCopy()
            .setSrcOffset(0)
            .setDstOffset(0)
            .setSize(dataSize);

        vk::CommandBuffer cmdBuffer = BeginSingleTimeCommands(deviceData);
        cmdBuffer.copyBuffer(stagingBuffer.get(), buffer.get(), region);
        EndSingleTimeCommands(deviceData, cmdBuffer);
    }
}

#endif
