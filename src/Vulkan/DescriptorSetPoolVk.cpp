#include "DescriptorSetPoolVk.h"

using namespace TinyRHI;

DescriptorSetPoolVk::DescriptorSetPoolVk(
    const DeviceData& _deviceData)
    : deviceData(_deviceData)
{
    auto uniformPoolSize = vk::DescriptorPoolSize()
        .setDescriptorCount(1000)
        .setType(vk::DescriptorType::eUniformBufferDynamic);	// uniform
    auto imageSamplerPoolSize = vk::DescriptorPoolSize()
        .setDescriptorCount(1000)
        .setType(vk::DescriptorType::eCombinedImageSampler);	// sampler2D��samplerCube
    auto storageImagePoolSize = vk::DescriptorPoolSize()
        .setDescriptorCount(1000)
        .setType(vk::DescriptorType::eStorageImage);	// image2D��imageCube
    auto storagePoolSize = vk::DescriptorPoolSize()
        .setDescriptorCount(1000)
        .setType(vk::DescriptorType::eStorageBuffer);

    std::vector<vk::DescriptorPoolSize> poolSizes = { uniformPoolSize, imageSamplerPoolSize, storageImagePoolSize, storagePoolSize };

    auto poolCreateInfo = vk::DescriptorPoolCreateInfo()
        .setMaxSets(1000)
        .setPoolSizeCount(poolSizes.size())
        .setPPoolSizes(poolSizes.data())
        .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);	// �ͷ�poolʱ�Զ��ͷ���Ӧset
    descriptorPool = deviceData.logicalDevice.createDescriptorPoolUnique(poolCreateInfo);
}

template <Bool bUniform>
Bool DescriptorSetWriterVk::WriteUniformBuffer(vk::Buffer buffer, Uint32 offset, Uint32 range, Uint32 dstBinding)
{
    auto bufferInfo = vk::DescriptorBufferInfo()
        .setBuffer(buffer)
        .setOffset(offset)
        .setRange(range);

    auto writeDescriptorSet = vk::WriteDescriptorSet()
        .setDstBinding(dstBinding)
        .setDescriptorCount(1)
        .setPBufferInfo(&bufferInfo);

    if (bUniform)
    {
        writeDescriptorSet.setDescriptorType(vk::DescriptorType::eUniformBuffer);
    }
    else
    {
        writeDescriptorSet.setDescriptorType(vk::DescriptorType::eStorageBuffer);
    }

    writeDescriptorSets.push_back(writeDescriptorSet);
    bDirty = true;
}

template <Bool bStorage>
Bool DescriptorSetWriterVk::WriteImageSampler(vk::ImageView imageView, vk::Sampler sampler, vk::ImageLayout layout, Uint32 dstBinding)
{
    auto imageInfo = vk::DescriptorImageInfo()
        .setImageView(imageView)
        .setSampler(sampler)
        .setImageLayout(layout);

    auto writeDescriptorSet = vk::WriteDescriptorSet()
        .setDstBinding(dstBinding)
        .setDescriptorCount(1)
        .setPImageInfo(&imageInfo);

    if (bStorage)
    {
        writeDescriptorSet.setDescriptorType(vk::DescriptorType::eStorageImage);
    }
    else
    {
        writeDescriptorSet.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
    }

    writeDescriptorSets.push_back(writeDescriptorSet);
    bDirty = true;
}