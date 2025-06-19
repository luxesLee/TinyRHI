#ifdef RHI_SUPPORT_VULKAN

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
        .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind);	// �ͷ�poolʱ�Զ��ͷ���Ӧset
    descriptorPool = deviceData.logicalDevice.createDescriptorPoolUnique(poolCreateInfo);
}

#endif
