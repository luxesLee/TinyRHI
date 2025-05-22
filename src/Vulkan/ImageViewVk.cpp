#include "ImageViewVk.h"

using namespace TinyRHI;

ImageVk::ImageVk(
    const DeviceData& _deviceData,
    const ImageDesc& _imageDesc)
    : deviceData(_deviceData), imageDesc(_imageDesc)
{
    this->size = imageDesc.size3[0] * imageDesc.size3[1] * imageDesc.size3[2];

    auto imageInfo = vk::ImageCreateInfo()
        .setImageType(ConvertImageType(imageDesc.imageType))
        .setExtent(vk::Extent3D(imageDesc.size3[0], imageDesc.size3[1], imageDesc.size3[2]))
        .setMipLevels(imageDesc.imageViewDesc.mipLevelsCount)
        .setArrayLayers(imageDesc.imageViewDesc.arrayLayersCount)
        .setFormat(ConvertFormat(imageDesc.format))
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setUsage(imageDesc.bStaging ? 
            vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled: 
            vk::ImageUsageFlagBits::eSampled)
        .setSharingMode(vk::SharingMode::eExclusive)
        .setSamples(ConvertMSAASamples(imageDesc.samples))
        .setFlags(vk::ImageCreateFlags());
    image = deviceData.logicalDevice.createImageUnique(imageInfo);

    vk::MemoryPropertyFlags memProp = imageDesc.bStaging ? vk::MemoryPropertyFlagBits::eDeviceLocal 
        : vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    vk::MemoryRequirements imageMemRequirements = deviceData.logicalDevice.getImageMemoryRequirements(image.get());
    vk::PhysicalDeviceMemoryProperties availableMemProperties = deviceData.physicalDevice.getMemoryProperties();
    auto allocInfo = vk::MemoryAllocateInfo()
        .setAllocationSize(imageMemRequirements.size)
        .setMemoryTypeIndex(findMemoryType(availableMemProperties, memProp, imageMemRequirements.memoryTypeBits));

    imageMemory = deviceData.logicalDevice.allocateMemoryUnique(allocInfo);

    deviceData.logicalDevice.bindImageMemory(image.get(), imageMemory.get(), 0);
}

void ImageVk::SetImageData(void* data, Uint32 dataSize)
{
    if (!imageDesc.bStaging)
    {
        void* mappedDataPtr = deviceData.logicalDevice.mapMemory(imageMemory.get(), 0, size);
        memcpy_s(static_cast<std::byte*>(mappedDataPtr), dataSize, data, dataSize);
        deviceData.logicalDevice.unmapMemory(imageMemory.get());
    }
    else
    {
        BufferDesc bufferDesc
        {
            .bufferType
            {
                .bTransfer = true
            },
            .elementNum = 1,
            .stride = dataSize,
            .bStaging = false,
        };
        std::unique_ptr<BufferVk> stagingBuffer = std::make_unique<BufferVk>(deviceData, bufferDesc);

        auto subresource = vk::ImageSubresource()
            .setMipLevel(imageDesc.imageViewDesc.mipLevelsCount)
            .setArrayLayer(imageDesc.imageViewDesc.arrayLayersCount)
            .setAspectMask(imageDesc.bDepth ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor);

        vk::BufferImageCopy region = vk::BufferImageCopy()
            .setBufferOffset(0)
            .setBufferRowLength(0)
            .setBufferImageHeight(0)
            .setImageOffset(vk::Offset3D(0, 0, 0))
            .setImageExtent(vk::Extent3D(imageDesc.size3[0], imageDesc.size3[1], imageDesc.size3[2]));

        TransitionImageLayout(deviceData, image.get(), imageDesc, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

        vk::CommandBuffer cmdBuffer = BeginSingleTimeCommands(deviceData);
        cmdBuffer.copyBufferToImage(stagingBuffer->BufferHandle(), image.get(), vk::ImageLayout::eTransferDstOptimal, {region});
        EndSingleTimeCommands(deviceData, cmdBuffer);

        TransitionImageLayout(deviceData, image.get(), imageDesc, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    }
}

ImageViewVk::ImageViewVk(
    const DeviceData& deviceData,
    const ImageDesc& imageDesc)
    : imagePtr(std::make_unique<ImageVk>(deviceData, imageDesc))
{
    vk::ImageViewType viewType = (imageDesc.imageType == ImageDesc::ImageType::e2D) ?
        vk::ImageViewType::e2D : vk::ImageViewType::e3D;
    vk::Format format = ConvertFormat(imageDesc.format);

    auto subresourceRange = vk::ImageSubresourceRange()
        .setAspectMask(imageDesc.bDepth ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor)
        .setBaseMipLevel(imageDesc.imageViewDesc.baseMipLevel)
        .setLevelCount(imageDesc.imageViewDesc.mipLevelsCount)
        .setBaseArrayLayer(imageDesc.imageViewDesc.baseArrayLayer)
        .setLayerCount(imageDesc.imageViewDesc.arrayLayersCount);

    auto imageViewInfo = vk::ImageViewCreateInfo()
        .setImage(imagePtr->ImageHandle())
        .setViewType(viewType)
        .setFormat(format)
        .setSubresourceRange(subresourceRange);
    imageView = deviceData.logicalDevice.createImageViewUnique(imageViewInfo);
}

ImageViewVk::ImageViewVk(const DeviceData &deviceData, const ImageDesc &imageDesc, vk::Image image)
{
    vk::ImageViewType viewType = (imageDesc.imageType == ImageDesc::ImageType::e2D) ?
        vk::ImageViewType::e2D : vk::ImageViewType::e3D;
    vk::Format format = ConvertFormat(imageDesc.format);

    auto subresourceRange = vk::ImageSubresourceRange()
        .setAspectMask(imageDesc.bDepth ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor)
        .setBaseMipLevel(imageDesc.imageViewDesc.baseMipLevel)
        .setLevelCount(imageDesc.imageViewDesc.mipLevelsCount)
        .setBaseArrayLayer(imageDesc.imageViewDesc.baseArrayLayer)
        .setLayerCount(imageDesc.imageViewDesc.arrayLayersCount);

    auto imageViewInfo = vk::ImageViewCreateInfo()
        .setImage(image)
        .setViewType(viewType)
        .setFormat(format)
        .setSubresourceRange(subresourceRange);
    imageView = deviceData.logicalDevice.createImageViewUnique(imageViewInfo);
}
