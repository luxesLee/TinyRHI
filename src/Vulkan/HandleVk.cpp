#include "HandleVk.h"
#include "BufferVk.h"
#include "ShaderVk.h"
#include "ImageViewVk.h"
#include "PipelineVk.h"
#include "PendingStateVk.h"
#include <iostream>

using namespace TinyRHI;

void VkHandle::InitVulkan()
{
    {
		auto appInfo = vk::ApplicationInfo()
			.setPApplicationName("TinyRHI")
			.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
			.setPEngineName("TinyRHI")
			.setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
			.setApiVersion(VK_API_VERSION_1_3);
		auto instanceCreateInfo = vk::InstanceCreateInfo()
			.setPApplicationInfo(&appInfo);
		instance = vk::createInstanceUnique(instanceCreateInfo);
	}

	{
		physicalDevices = instance->enumeratePhysicalDevices();
		std::cout << "Found " << physicalDevices.size() << " physical device(s)\n";
		for (const auto& pDevice : physicalDevices)
		{
			vk::PhysicalDeviceProperties deviceProperties = pDevice.getProperties();
			std::cout << "  Physical device found: " << deviceProperties.deviceName;
			vk::PhysicalDeviceFeatures deviceFeatures = pDevice.getFeatures();

			if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			{
				deviceData.physicalDevice = pDevice;
				break;
			}
		}
	}

	{
		std::vector<vk::QueueFamilyProperties> queueFamilies = deviceData.physicalDevice.getQueueFamilyProperties();
		for (uint32_t index = 0; index < queueFamilies.size(); index++)
		{
			if (queueFamilies[index].queueFlags & vk::QueueFlagBits::eGraphics && deviceData.queueFamilyIndices.graphicsFamilyIndex == -1)
			{
				deviceData.queueFamilyIndices.graphicsFamilyIndex = index;
			}

			// if (selectPhysicalDevice.getSurfaceSupportKHR(index, surface.get()) && queueFamilyIndices.presentFamilyIndex == -1)
			// {
			// 	queueFamilyIndices.presentFamilyIndex = index;
			// }
		}

		std::vector<uint32_t> uniqueQueueFamilyIndices =
		{
			deviceData.queueFamilyIndices.graphicsFamilyIndex,
			deviceData.queueFamilyIndices.presentFamilyIndex
		};
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilyIndices)
		{
			auto queueCreateInfo = vk::DeviceQueueCreateInfo()
				.setQueueFamilyIndex(queueFamily)
				.setQueueCount(1)
				.setPQueuePriorities(&queuePriority);

			queueCreateInfos.push_back(queueCreateInfo);
		}

		auto deviceFeatures = vk::PhysicalDeviceFeatures();

		auto deviceCreateInfo = vk::DeviceCreateInfo()
			.setQueueCreateInfoCount((uint32_t)queueCreateInfos.size())
			.setPQueueCreateInfos(queueCreateInfos.data())
			.setPEnabledFeatures(&deviceFeatures);

		deviceData.logicalDevice = deviceData.physicalDevice.createDevice(deviceCreateInfo);
		deviceData.graphicsQueue = deviceData.logicalDevice.getQueue(deviceData.queueFamilyIndices.graphicsFamilyIndex, 0);
		deviceData.presentQueue = deviceData.logicalDevice.getQueue(deviceData.queueFamilyIndices.presentFamilyIndex, 0);
	}

	cmdPoolManager = std::make_unique<CommandPoolManager>(deviceData);
	deviceData.commandPool = cmdPoolManager->CmdPoolHandle();
}

IGraphicsPipeline* VkHandle::CreateGrpahicsPipeline(const GraphicsPipelineDesc &gfxPipelineDesc)
{
	return new GraphicsPipelineVk(deviceData, gfxPipelineDesc);
}

IComputePipeline* VkHandle::CreateComputePipeline(const ComputePipelineDesc &computePipelineDesc)
{
    return new ComputePipelineVk(deviceData, computePipelineDesc);
}

IShader *VkHandle::CreateVertexShader(ShaderDesc &shaderDesc)
{
	return new ShaderVk<IShader::Stage::Vertex>(deviceData, shaderDesc);
}

IShader* VkHandle::CreatePixelShader(ShaderDesc &shaderDesc)
{
    return new ShaderVk<IShader::Stage::Pixel>(deviceData, shaderDesc);
}

IShader* VkHandle::CreateComputeShader(ShaderDesc &shaderDesc)
{
    return new ShaderVk<IShader::Stage::Compute>(deviceData, shaderDesc);
}

IBuffer* VkHandle::CreateBuffer(const BufferDesc &bufferDesc)
{
    return new BufferVk(deviceData, bufferDesc);
}

IBuffer* VkHandle::CreateBufferWithData(const BufferDesc &bufferDesc, void *data, Uint32 dataSize)
{
	auto pVkBuffer = new BufferVk(deviceData, bufferDesc);
	pVkBuffer->SetBufferData(data, dataSize, 0);
    return pVkBuffer;
}

IImageView* VkHandle::CreateImageView(const ImageDesc &imageDesc)
{
    return new ImageViewVk(deviceData, imageDesc);
}

IImageView* VkHandle::CreateImageViewWithData(const ImageDesc &imageDesc, void *data, Uint32 dataSize)
{
	auto pVkImageView = new ImageViewVk(deviceData, imageDesc);
	pVkImageView->SetImageData(data, dataSize);
    return pVkImageView;
}

Uint32 VkHandle::GetTotalVRAM() const
{
	vk::PhysicalDeviceMemoryProperties deviceMemoryProperties = deviceData.physicalDevice.getMemoryProperties();
	Uint32 totalMemory = 0;
	for(Uint32 i = 0; i < deviceMemoryProperties.memoryHeapCount; i++)
	{
        if (deviceMemoryProperties.memoryHeaps[i].flags & vk::MemoryHeapFlagBits::eDeviceLocal) {
            totalMemory += deviceMemoryProperties.memoryHeaps[i].size;
        }
	}

    return totalMemory / (1024 * 1024);
}

Uint32 VkHandle::GetUsedVRAM() const
{
    return 0;
}

void VkHandle::BeginFrame()
{
}

void VkHandle::EndFrame()
{
}

void VkHandle::BeginCommand()
{
	currentCmd = cmdPoolManager->GetCmdBuffer();
	currentCmd.begin(vk::CommandBufferBeginInfo());
}

void VkHandle::EndCommand()
{
	currentCmd.end();

	currentCmd = VK_NULL_HANDLE;
}

void VkHandle::BeginRenderPass()
{
	if(!currentCmd)
	{
		
	}
	vk::RenderPassBeginInfo renderPassBeginInfo = vk::RenderPassBeginInfo()
		// .setRenderPass()
		// .setFramebuffer()
		// .setRenderArea()
		// .setClearValueCount()
		// .setClearValues()
		;
	currentCmd.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
}

void VkHandle::EndRenderPass()
{
	currentCmd.endRenderPass();
}

void VkHandle::Commit()
{
	// cmdPoolManager->SubmitCmdBuffer();
}

void VkHandle::SetGraphicsPipelineState(IGraphicsPipeline* gfxPipeline)
{
	GraphicsPipelineVk* vkGfxPipeline = dynamic_cast<GraphicsPipelineVk*>(gfxPipeline);
	if(vkGfxPipeline)
	{
		if(pGfxPending->SetPipeline(vkGfxPipeline))
		{
			vkGfxPipeline->Bind(currentCmd);
			pGfxPending->MarkUpdateDynamicStates();
		}
	}
}

void VkHandle::SetComputePipelineState(IComputePipeline *computePipeline)
{
	ComputePipelineVk* vkComputePipeline = dynamic_cast<ComputePipelineVk*>(computePipeline);
	if(vkComputePipeline)
	{
		if(pComputePending->SetPipeline(vkComputePipeline))
		{
			vkComputePipeline->Bind(currentCmd);
		}
	}
}

void VkHandle::SetVertexStream(IBuffer *buffer, Uint32 offset)
{
	pGfxPending->SetBuffer();
}

void VkHandle::SetViewport(Extent3D minExt, Extent3D maxExt)
{
	vk::Viewport viewport;
	viewport.setMinDepth(minExt.depth).setMaxDepth(maxExt.depth);
	viewport.setX(minExt.width).setY(minExt.height);
	viewport.setWidth(maxExt.width - minExt.width).setHeight(maxExt.height - minExt.height);
	pGfxPending->SetViewport(viewport);
}

void VkHandle::SetScissor(Extent2D minExt, Extent2D maxExt)
{
	vk::Rect2D rect2D;
	rect2D.setOffset(vk::Offset2D(minExt.width, minExt.height));
	rect2D.setExtent(vk::Extent2D(maxExt.width - minExt.width, maxExt.height - minExt.height));
	pGfxPending->SetScissor(rect2D);
}

void VkHandle::DrawPrimitive(Uint32 baseVertexIndex, Uint32 numPrimitives, Uint32 numInstances)
{
	pGfxPending->PrepareDraw();

	numInstances = std::max(1U, numInstances);
	Uint numVertices;

	currentCmd.draw(numVertices, numInstances, baseVertexIndex, 0);
}

void VkHandle::DrawPrimitiveIndirect(IBuffer *argumentBuffer, Uint32 argumentOffset)
{
	pGfxPending->PrepareDraw();
	BufferVk* vkArgumentBuffer = dynamic_cast<BufferVk*>(argumentBuffer);
	if(vkArgumentBuffer)
	{
		currentCmd.drawIndirect(vkArgumentBuffer->BufferHandle(), 0, 1, sizeof(vk::DrawIndirectCommand));
	}
}

void VkHandle::DrawIndexPrimitive(IBuffer *indexBuffer, Int32 baseVertexIndex, Uint32 firstInstance, Uint32 startIndex, Uint32 numPrimitives, Uint32 numInstances)
{
	pGfxPending->PrepareDraw();
	BufferVk* vkIndexBuffer = dynamic_cast<BufferVk*>(indexBuffer);
	if(vkIndexBuffer)
	{
		currentCmd.bindIndexBuffer(vkIndexBuffer->BufferHandle(), 0, vk::IndexType::eUint32);
		Uint32 numIndices;
		currentCmd.drawIndexed(numIndices, numInstances, startIndex, baseVertexIndex, firstInstance);
	}
}

void VkHandle::Dispatch(Uint32 threadGroupCountX, Uint32 threadGroupCountY, Uint32 threadGroupCountZ)
{
	pComputePending->PrepareDispatch();
	currentCmd.dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void VkHandle::UpdateBuffer(IBuffer *buffer, void *data, Uint32 dataSize, Uint32 offset)
{
	BufferVk* vkBuffer = dynamic_cast<BufferVk*>(buffer);
	if(vkBuffer)
	{
		vkBuffer->UpdateBufferData(data, dataSize, offset);
	}
}

void VkHandle::UpdateImageView(IImageView *imageView, void *data, Uint32 dataSize)
{
	ImageViewVk* vkImageView = dynamic_cast<ImageViewVk*>(imageView);
	if(vkImageView)
	{
		vkImageView->SetImageData(data, dataSize);
	}
}

void VkHandle::CopyBuffer(IBuffer *srcBuffer, IBuffer *dstBuffer)
{
	BufferVk* vkSrcBuffer = dynamic_cast<BufferVk*>(srcBuffer);
	BufferVk* vkDstBuffer = dynamic_cast<BufferVk*>(dstBuffer);
	if(vkSrcBuffer && vkDstBuffer && vkDstBuffer->GetSize() >= vkSrcBuffer->GetSize())
	{
		vk::BufferCopy region = vk::BufferCopy()
            .setSrcOffset(0)
            .setDstOffset(0)
            .setSize(vkSrcBuffer->GetSize());
        vk::CommandBuffer cmdBuffer = BeginSingleTimeCommands(deviceData);
        cmdBuffer.copyBuffer(vkSrcBuffer->BufferHandle(), vkDstBuffer->BufferHandle(), region);
        EndSingleTimeCommands(deviceData, cmdBuffer);
	}
}

void VkHandle::CopyBufferToImage(IBuffer *srcBuffer, IImageView *dstImageView)
{
	BufferVk* vkSrcBuffer = dynamic_cast<BufferVk*>(srcBuffer);
	ImageViewVk* vkDstImageView = dynamic_cast<ImageViewVk*>(dstImageView);
	if(vkSrcBuffer && vkDstImageView && vkDstImageView->GetSize() >= vkSrcBuffer->GetSize())
	{
		const auto& imageDesc = vkDstImageView->DescHandle();
        vk::BufferImageCopy region = vk::BufferImageCopy()
            .setBufferOffset(0)
            .setBufferRowLength(0)
            .setBufferImageHeight(0)
            .setImageOffset(vk::Offset3D(0, 0, 0))
            .setImageExtent(vk::Extent3D(imageDesc.size3[0], imageDesc.size3[1], imageDesc.size3[2]));

        TransitionImageLayout(deviceData, vkDstImageView->ImageHandle(), imageDesc, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

        vk::CommandBuffer cmdBuffer = BeginSingleTimeCommands(deviceData);
        cmdBuffer.copyBufferToImage(vkSrcBuffer->BufferHandle(), vkDstImageView->ImageHandle(), vk::ImageLayout::eTransferDstOptimal, {region});
        EndSingleTimeCommands(deviceData, cmdBuffer);

        TransitionImageLayout(deviceData, vkDstImageView->ImageHandle(), imageDesc, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
	}
}

void VkHandle::CopyImageToBuffer(IImageView *srcImageView, IBuffer *dstBuffer)
{
	ImageViewVk* vkSrcImageView = dynamic_cast<ImageViewVk*>(srcImageView);
	BufferVk* vkDstBuffer = dynamic_cast<BufferVk*>(dstBuffer);
	if(vkSrcImageView && vkDstBuffer && vkDstBuffer->GetSize() >= vkSrcImageView->GetSize())
	{
		const auto& imageDesc = vkSrcImageView->DescHandle();
        vk::BufferImageCopy region = vk::BufferImageCopy()
            .setBufferOffset(0)
            .setBufferRowLength(0)
            .setBufferImageHeight(0)
            .setImageOffset(vk::Offset3D(0, 0, 0))
            .setImageExtent(vk::Extent3D(imageDesc.size3[0], imageDesc.size3[1], imageDesc.size3[2]));

        TransitionImageLayout(deviceData, vkSrcImageView->ImageHandle(), imageDesc, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferSrcOptimal);

        vk::CommandBuffer cmdBuffer = BeginSingleTimeCommands(deviceData);
        cmdBuffer.copyImageToBuffer(vkSrcImageView->ImageHandle(), vk::ImageLayout::eTransferSrcOptimal, vkDstBuffer->BufferHandle(), {region});
        EndSingleTimeCommands(deviceData, cmdBuffer);

        TransitionImageLayout(deviceData, vkSrcImageView->ImageHandle(), imageDesc, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
	}
}

void VkHandle::CopyImageToImage(IImageView *srcImageView, IImageView *dstImageView)
{
	ImageViewVk* vkSrcImageView = dynamic_cast<ImageViewVk*>(srcImageView);
	ImageViewVk* vkDstImageView = dynamic_cast<ImageViewVk*>(dstImageView);
	if(vkSrcImageView && vkDstImageView && vkDstImageView->GetSize() >= vkSrcImageView->GetSize())
	{
		const auto& srcImageDesc = vkSrcImageView->DescHandle();
		const auto& dstImageDesc = vkDstImageView->DescHandle();
        vk::ImageCopy region = vk::ImageCopy()
			.setSrcOffset(0)
			.setSrcSubresource(vk::ImageSubresourceLayers()
				.setMipLevel(srcImageDesc.imageViewDesc.mipLevelsCount)
				.setBaseArrayLayer(srcImageDesc.imageViewDesc.arrayLayersCount)
				.setAspectMask(srcImageDesc.bDepth ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor))
			.setDstOffset(0)
			.setDstSubresource(vk::ImageSubresourceLayers()
				.setMipLevel(dstImageDesc.imageViewDesc.mipLevelsCount)
				.setBaseArrayLayer(dstImageDesc.imageViewDesc.arrayLayersCount)
				.setAspectMask(dstImageDesc.bDepth ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor))
			.setExtent(vk::Extent3D(srcImageDesc.size3[0], srcImageDesc.size3[1], srcImageDesc.size3[2]));

        TransitionImageLayout(deviceData, vkSrcImageView->ImageHandle(), srcImageDesc, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferSrcOptimal);
        TransitionImageLayout(deviceData, vkDstImageView->ImageHandle(), dstImageDesc, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

        vk::CommandBuffer cmdBuffer = BeginSingleTimeCommands(deviceData);
		cmdBuffer.copyImage(vkSrcImageView->ImageHandle(), vk::ImageLayout::eTransferSrcOptimal, vkDstImageView->ImageHandle(), vk::ImageLayout::eTransferDstOptimal, {region});
        EndSingleTimeCommands(deviceData, cmdBuffer);

        TransitionImageLayout(deviceData, vkSrcImageView->ImageHandle(), srcImageDesc, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
		TransitionImageLayout(deviceData, vkDstImageView->ImageHandle(), dstImageDesc, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
	}
}
