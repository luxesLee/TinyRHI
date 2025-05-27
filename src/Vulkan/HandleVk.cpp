#include <iostream>
#include <set>
#include <cassert>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "HandleVk.h"
#include "BufferVk.h"
#include "ShaderVk.h"
#include "ImageViewVk.h"
#include "PipelineVk.h"

using namespace TinyRHI;

VkHandle::VkHandle(GLFWwindow *_window)
	: window(_window)
{
	InitVulkan();
	InitPendingState();
}

void VkHandle::InitVulkan()
{
	InitInstanceAndPhysicalDevice();
	InitSurface();
	InitDevice();
	InitSwapChain();
	InitSync();
	cmdPoolManager = std::make_unique<CommandPoolManager>(deviceData);
	deviceData.commandPool = cmdPoolManager->CmdPoolHandle();
}

void VkHandle::InitInstanceAndPhysicalDevice()
{
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		std::vector<const char*> validationLayers;

#ifdef DEBUG_VULKAN_MACRO
		validationLayers.push_back("VK_LAYER_KHRONOS_validation");
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		auto appInfo = vk::ApplicationInfo()
			.setPApplicationName("TinyRHI")
			.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
			.setPEngineName("TinyRHI")
			.setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
			.setApiVersion(VK_API_VERSION_1_3);
		auto instanceCreateInfo = vk::InstanceCreateInfo()
			.setPApplicationInfo(&appInfo)
			.setEnabledExtensionCount(extensions.size())
			.setPpEnabledExtensionNames(extensions.data())
			.setEnabledLayerCount(validationLayers.size())
			.setPpEnabledLayerNames(validationLayers.data());
		instance = vk::createInstanceUnique(instanceCreateInfo);

#ifdef DEBUG_VULKAN_MACRO
		vk::DebugUtilsMessengerCreateInfoEXT createInfo = vk::DebugUtilsMessengerCreateInfoEXT()
			.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning 
				| vk::DebugUtilsMessageSeverityFlagBitsEXT::eError 
				| vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
			.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral 
				| vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance 
				| vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
			.setPfnUserCallback(DebugMessageCallback)
			.setPUserData(nullptr);
	
		loader = vk::DispatchLoaderDynamic(instance.get(), vkGetInstanceProcAddr);
		debugUtilsMessenger = instance->createDebugUtilsMessengerEXTUnique(createInfo, nullptr, loader);
#endif
	}

	{
		physicalDevices = instance->enumeratePhysicalDevices();
		// std::cout << "Found " << physicalDevices.size() << " physical device(s)\n";
		for (const auto& pDevice : physicalDevices)
		{
			vk::PhysicalDeviceProperties deviceProperties = pDevice.getProperties();
			// std::cout << "  Physical device found: " << deviceProperties.deviceName;
			vk::PhysicalDeviceFeatures deviceFeatures = pDevice.getFeatures();

			if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			{
				deviceData.physicalDevice = pDevice;
				break;
			}
		}
	}
}

void VkHandle::InitSurface()
{
	if(glfwCreateWindowSurface(instance.get(), window, nullptr, &surface) != VK_SUCCESS)
	{
		assert(false);
	}
}

void VkHandle::InitDevice()
{
	deviceData.queueFamilyIndices.graphicsFamilyIndex = -1;
	deviceData.queueFamilyIndices.presentFamilyIndex = -1;
	std::vector<vk::QueueFamilyProperties> queueFamilies = deviceData.physicalDevice.getQueueFamilyProperties();
	for (uint32_t index = 0; index < queueFamilies.size(); index++)
	{
		if (queueFamilies[index].queueFlags & vk::QueueFlagBits::eGraphics 
			&& queueFamilies[index].queueCount > 0
			&& deviceData.queueFamilyIndices.graphicsFamilyIndex == Uint32(-1))
		{
			deviceData.queueFamilyIndices.graphicsFamilyIndex = index;
		}

		if (deviceData.physicalDevice.getSurfaceSupportKHR(index, surface) 
			&& queueFamilies[index].queueCount > 0
			&& deviceData.queueFamilyIndices.presentFamilyIndex == Uint32(-1))
		{
			deviceData.queueFamilyIndices.presentFamilyIndex = index;
		}
	}
	assert(deviceData.queueFamilyIndices.graphicsFamilyIndex != Uint32(-1));
	assert(deviceData.queueFamilyIndices.presentFamilyIndex != Uint32(-1));

	std::set<Uint32> uniqueQueueFamilyIndices =
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

	std::vector<const char*> deviceExtensions = 
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

#ifdef DEBUG_VULKAN_MACRO
	std::vector<const char*> validationLayers;
	validationLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

	auto deviceCreateInfo = vk::DeviceCreateInfo()
		.setQueueCreateInfoCount((uint32_t)queueCreateInfos.size())
		.setPQueueCreateInfos(queueCreateInfos.data())
		.setPEnabledFeatures(&deviceFeatures)
#ifdef DEBUG_VULKAN_MACRO
		.setEnabledLayerCount(validationLayers.size())
		.setPpEnabledLayerNames(validationLayers.data())
#else
		.setEnabledLayerCount(0)
#endif
		.setEnabledExtensionCount(deviceExtensions.size())
		.setPpEnabledExtensionNames(deviceExtensions.data());

	deviceData.logicalDevice = deviceData.physicalDevice.createDevice(deviceCreateInfo);
	deviceData.graphicsQueue = deviceData.logicalDevice.getQueue(deviceData.queueFamilyIndices.graphicsFamilyIndex, 0);
	deviceData.presentQueue = deviceData.logicalDevice.getQueue(deviceData.queueFamilyIndices.presentFamilyIndex, 0);
}

void VkHandle::InitSwapChain()
{
	auto capabilities = deviceData.physicalDevice.getSurfaceCapabilitiesKHR(surface);
	auto formats = deviceData.physicalDevice.getSurfaceFormatsKHR(surface);
	auto presentModes = deviceData.physicalDevice.getSurfacePresentModesKHR(surface);

	auto getSwapChainSurfaceFormt = [&](const std::vector<vk::SurfaceFormatKHR>& availableFormats)
	{
		vk::SurfaceFormatKHR bestFormat = { vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear };
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == bestFormat.format && availableFormat.colorSpace == bestFormat.colorSpace) {
				return availableFormat;
			}
		}
		return bestFormat;
	};

	auto getSwapPresentMode = [&](const std::vector<vk::PresentModeKHR>& availablePresentModes, vk::PresentModeKHR preferredMode)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == preferredMode)
			{
				return preferredMode;
			}
		}
		return vk::PresentModeKHR::eFifo;
	};

	auto getSwapExtent = [&](const vk::SurfaceCapabilitiesKHR& capabilities) -> vk::Extent2D
	{
		assert(capabilities.currentExtent.width != (std::numeric_limits<Uint32>::max)());
		return capabilities.currentExtent;
	};

	auto surfaceFormat = getSwapChainSurfaceFormt(formats);
	auto presentMode = getSwapPresentMode(presentModes, vk::PresentModeKHR::eMailbox);
	swapChainExtent = getSwapExtent(capabilities);
	Uint32 imageCount = (std::max)(capabilities.minImageCount, 2U);

	auto createInfo = vk::SwapchainCreateInfoKHR()
		.setSurface(surface)
		.setPreTransform(capabilities.currentTransform)
		.setImageFormat(surfaceFormat.format)
		.setImageColorSpace(surfaceFormat.colorSpace)
		.setPresentMode(presentMode)
		.setImageExtent(swapChainExtent)
		.setImageArrayLayers(1)
		.setMinImageCount(imageCount)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
		.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
		.setClipped(true)
		;
	
	uint32_t familyIndices[] = { deviceData.queueFamilyIndices.graphicsFamilyIndex, deviceData.queueFamilyIndices.presentFamilyIndex };
	if(deviceData.queueFamilyIndices.graphicsFamilyIndex != deviceData.queueFamilyIndices.presentFamilyIndex)
	{
		createInfo.setImageSharingMode(vk::SharingMode::eConcurrent)
			.setQueueFamilyIndexCount(2)
			.setPQueueFamilyIndices(familyIndices);
	}
	else
	{
		createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
	}

	swapChain = deviceData.logicalDevice.createSwapchainKHRUnique(createInfo);

	std::vector<vk::Image> swapImages = deviceData.logicalDevice.getSwapchainImagesKHR(swapChain.get());
	assert(swapImages.size() > 0);

	swapImageViews.clear();
	ImageDesc imageDesc
	{
		.size3 = {swapChainExtent.width, swapChainExtent.height, 1},
		.format = Format::BGRA8_SRGB,
		.usage
		{
			.colorAttach = true,
		}
	};
	for(Uint i = 0; i < swapImages.size(); i++)
	{
		swapImageViews.push_back(std::make_unique<ImageViewVk>(deviceData, imageDesc, swapImages[i]));
	}
}

void VkHandle::InitSync()
{
	currentFrame = 0;
	swapImageAvailableSemaphores.resize(2);
	renderFinishedSemaphores.resize(2);
	inFlightFences.resize(2);
	auto fenceCreateInfo = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);
	for(Uint i = 0; i < 2; i++)
	{
		swapImageAvailableSemaphores[i] = deviceData.logicalDevice.createSemaphoreUnique(vk::SemaphoreCreateInfo());
		renderFinishedSemaphores[i] = deviceData.logicalDevice.createSemaphoreUnique(vk::SemaphoreCreateInfo());
		inFlightFences[i] = deviceData.logicalDevice.createFenceUnique(fenceCreateInfo);
	}
}

#ifdef DEBUG_VULKAN_MACRO
VKAPI_ATTR VkBool32 VKAPI_CALL VkHandle::DebugMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}
#endif










IShader *VkHandle::CreateVertexShader(const ShaderDesc &shaderDesc)
{
	return new ShaderVk<IShader::Stage::Vertex>(deviceData, shaderDesc);
}

IShader* VkHandle::CreatePixelShader(const ShaderDesc &shaderDesc)
{
    return new ShaderVk<IShader::Stage::Pixel>(deviceData, shaderDesc);
}

IShader* VkHandle::CreateComputeShader(const ShaderDesc &shaderDesc)
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

ITexture* VkHandle::CreateTexture(const ImageDesc& imageDesc, const SamplerState& samplerState)
{
    return new TextureVk(deviceData, imageDesc, samplerState);
}

ITexture *VkHandle::CreateTextureWithoutSampling(const ImageDesc &imageDesc)
{
    return new TextureVk(deviceData, imageDesc);
}

ITexture* VkHandle::CreateTextureWithData(const ImageDesc& imageDesc, const SamplerState& samplerState, void* data, Uint32 dataSize)
{
	auto pVkTexture = new TextureVk(deviceData, imageDesc, samplerState);
	pVkTexture->SetImageData(data, dataSize);
    return pVkTexture;
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








IRHIHandle* VkHandle::BeginFrame()
{
	vk::Result waitFenceResult = deviceData.logicalDevice.waitForFences(inFlightFences[currentFrame].get(), true, UINT64_MAX);

	vk::ResultValue result = deviceData.logicalDevice.acquireNextImageKHR(
		swapChain.get(), UINT64_MAX, swapImageAvailableSemaphores[currentFrame].get(), nullptr);
	swapImageIndex = result.value;

	deviceData.logicalDevice.resetFences(inFlightFences[currentFrame].get());
	return this;
}

IRHIHandle* VkHandle::EndFrame()
{
	vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR()
		.setSwapchainCount(1)
		.setPSwapchains(&swapChain.get())
		.setPImageIndices(&swapImageIndex)
		.setWaitSemaphoreCount(1)
		.setPWaitSemaphores(&renderFinishedSemaphores[currentFrame].get())
		;
	
	auto result = deviceData.presentQueue.presentKHR(presentInfo);
	assert(result == vk::Result::eSuccess);
	swapImageIndex = -1;
	currentFrame = (currentFrame + 1) % 2;
	return this;
}

IRHIHandle* VkHandle::BeginCommand()
{
	// todo: Multi-threaded command
	currentCmd = cmdPoolManager->GetCmdBuffer(currentFrame);
	currentCmd.reset();
	currentCmd.begin(vk::CommandBufferBeginInfo());
	renderResManager->ClearAttachments();
	return this;
}

IRHIHandle* VkHandle::EndCommand()
{
	currentCmd.end();
	currentCmd = VK_NULL_HANDLE;
	return this;
}

IRHIHandle* VkHandle::Commit()
{
	cmdPoolManager->SubmitCmdBuffer(
		currentFrame,
		{swapImageAvailableSemaphores[currentFrame].get()},
		{renderFinishedSemaphores[currentFrame].get()},
		vk::PipelineStageFlagBits::eColorAttachmentOutput,
		inFlightFences[currentFrame].get());
	return this;
}

IRHIHandle* VkHandle::BeginRenderPass()
{
	renderResManager->BeginRenderPass(currentCmd);
	return this;
}

IRHIHandle* VkHandle::EndRenderPass()
{
	renderResManager->EndRenderPass(currentCmd);
	pGfxPending->Reset();
	return this;
}

IRHIHandle* VkHandle::SetGraphicsPipeline(const GfxSetting& gfxSetting)
{
	PipelineLayoutVk* pipelineLayout = pGfxPending->GetPipelineLayout(deviceData);
	GraphicsPipelineVk* vkGfxPipeline = renderResManager->GetGfxPipeline(gfxSetting, pipelineLayout);
	if(vkGfxPipeline)
	{
		if(pGfxPending->SetPipeline(vkGfxPipeline))
		{
			bCurrentGfx = true;
			pGfxPending->SetCmdBuffer(currentCmd);
			pGfxPending->Bind();
			pGfxPending->MarkUpdateDynamicStates();
		}
	}
	return this;
}

IRHIHandle* VkHandle::SetComputePipeline()
{
	PipelineLayoutVk* pipelineLayout = pComputePending->GetPipelineLayout(deviceData);
	ComputePipelineVk* vkComputePipeline = renderResManager->GetComputePipeline(pipelineLayout);
	if(vkComputePipeline)
	{
		if(pComputePending->SetPipeline(vkComputePipeline))
		{
			bCurrentGfx = false;
			pComputePending->Bind();
			pComputePending->SetCmdBuffer(currentCmd);
		}
	}
	return this;
}

IRHIHandle* VkHandle::SetDefaultAttachments(const AttachmentDesc &attachmentDesc)
{
	std::shared_ptr<AttachmentVk> colorAttach = std::make_shared<AttachmentVk>(swapImageViews[currentFrame].get(), attachmentDesc, false);
	renderResManager->SetColorAttachments(colorAttach);
    return this;
}

IRHIHandle *VkHandle::SetColorAttachments(ITexture *texture, const AttachmentDesc &attachmentDesc)
{
	TextureVk* vkTexture = dynamic_cast<TextureVk*>(texture);
	if(vkTexture)
	{
		std::shared_ptr<AttachmentVk> colorAttach = std::make_shared<AttachmentVk>(vkTexture->ImageViewPtr(), attachmentDesc, false);
		renderResManager->SetColorAttachments(colorAttach);
	}
    return this;
}

IRHIHandle* VkHandle::SetDepthAttachment(ITexture *texture, const AttachmentDesc& attachmentDesc)
{
	TextureVk* vkTexture = dynamic_cast<TextureVk*>(texture);
	if(vkTexture)
	{
		std::shared_ptr<AttachmentVk> depthAttach = std::make_shared<AttachmentVk>(vkTexture->ImageViewPtr(), attachmentDesc, true);
		renderResManager->SetDepthAttachment(depthAttach);
	}
    return this;
}

IRHIHandle* VkHandle::SetVertexShader(IShader *shader)
{
	assert(shader);
	renderResManager->SetShader<IShader::Stage::Vertex>(shader);
    return this;
}

IRHIHandle* VkHandle::SetPixelShader(IShader *shader)
{
	assert(shader);
	renderResManager->SetShader<IShader::Stage::Pixel>(shader);
    return this;
}

IRHIHandle* VkHandle::SetComputeShader(IShader *shader)
{
	assert(shader);
	renderResManager->SetShader<IShader::Stage::Compute>(shader);
    return this;
}

IRHIHandle *VkHandle::SetVertexStream(Uint32 vertId, IBuffer *buffer, Uint32 offset)
{
	BufferVk* vkBuffer = dynamic_cast<BufferVk*>(buffer);
	if(vkBuffer)
	{
		pGfxPending->SetVertex(vertId, vkBuffer->BufferHandle(), offset);
	}
	return this;
}

IRHIHandle* VkHandle::SetViewport(Extent2D minExt, Extent2D maxExt)
{
	vk::Viewport viewport;
	viewport.setMinDepth(0).setMaxDepth(0);
	viewport.setX(minExt.width).setY(minExt.height);
	viewport.setWidth(maxExt.width - minExt.width).setHeight(maxExt.height - minExt.height);
	pGfxPending->SetViewport(viewport);
	return this;
}

IRHIHandle* VkHandle::SetViewport(Extent3D minExt, Extent3D maxExt)
{
	vk::Viewport viewport;
	viewport.setMinDepth(minExt.depth).setMaxDepth(maxExt.depth);
	viewport.setX(minExt.width).setY(minExt.height);
	viewport.setWidth(maxExt.width - minExt.width).setHeight(maxExt.height - minExt.height);
	pGfxPending->SetViewport(viewport);
	return this;
}

IRHIHandle* VkHandle::SetScissor(Extent2D minExt, Extent2D maxExt)
{
	vk::Rect2D rect2D;
	rect2D.setOffset(vk::Offset2D(minExt.width, minExt.height));
	rect2D.setExtent(vk::Extent2D(maxExt.width - minExt.width, maxExt.height - minExt.height));
	pGfxPending->SetScissor(rect2D);
	return this;
}

IRHIHandle* VkHandle::SetSamplerTexture(ITexture *texture, IShader::Stage stage, Uint setId, Uint bindingId)
{
	TextureVk* vkTexture = dynamic_cast<TextureVk*>(texture);
	if(vkTexture)
	{
		if(bCurrentGfx)
		{
			pGfxPending->SetSamplerImage(vkTexture, stage, setId, bindingId);
		}
		else
		{
			pComputePending->SetSamplerImage(vkTexture, stage, setId, bindingId);
		}
	}
	return this;
}

IRHIHandle* VkHandle::SetStorageTexture(ITexture *texture, IShader::Stage stage, Uint setId, Uint bindingId)
{
	TextureVk* vkTexture = dynamic_cast<TextureVk*>(texture);
	if(vkTexture)
	{
		if(bCurrentGfx)
		{
			pGfxPending->SetStorageImage(vkTexture, stage, setId, bindingId);
		}
		else
		{
			pComputePending->SetStorageImage(vkTexture, stage, setId, bindingId);
		}
	}
	return this;
}

IRHIHandle* VkHandle::SetStorageBuffer(IBuffer *buffer, IShader::Stage stage, Uint setId, Uint bindingId)
{
	BufferVk* vkBuffer = dynamic_cast<BufferVk*>(buffer);
	if(vkBuffer)
	{
		if(bCurrentGfx)
		{
			pGfxPending->SetStorageBuffer(vkBuffer, stage, setId, bindingId);
		}
		else
		{
			pComputePending->SetStorageBuffer(vkBuffer, stage, setId, bindingId);
		}
	}
	return this;
}

IRHIHandle* VkHandle::SetUniformBuffer(IBuffer *buffer, IShader::Stage stage, Uint setId, Uint bindingId)
{
	BufferVk* vkBuffer = dynamic_cast<BufferVk*>(buffer);
	if(vkBuffer)
	{
		if(bCurrentGfx)
		{
			pGfxPending->SetUniformBuffer(vkBuffer, stage, setId, bindingId);
		}
		else
		{
			pComputePending->SetUniformBuffer(vkBuffer, stage, setId, bindingId);
		}
	}
	return this;
}

IRHIHandle* VkHandle::DrawPrimitive(Uint32 vertexCount, Uint32 firstVertex)
{
	pGfxPending->PrepareDraw();
	// #1: vert count per instance
	// #2: instance count
	// #3: ignore first #3 num vertices
	// #4: same #3 but instance
	currentCmd.draw(vertexCount, 1, firstVertex, 0);
	return this;
}

IRHIHandle* VkHandle::DrawPrimitiveIndirect(IBuffer *argumentBuffer, Uint32 argumentOffset)
{
	pGfxPending->PrepareDraw();
	BufferVk* vkArgumentBuffer = dynamic_cast<BufferVk*>(argumentBuffer);
	if(vkArgumentBuffer)
	{
		currentCmd.drawIndirect(vkArgumentBuffer->BufferHandle(), 0, 1, sizeof(vk::DrawIndirectCommand));
	}
	return this;
}

IRHIHandle* VkHandle::DrawIndexPrimitive(IBuffer *indexBuffer, Int32 baseVertexIndex, Uint32 firstInstance, Uint32 startIndex, Uint32 numPrimitives, Uint32 numInstances)
{
	pGfxPending->PrepareDraw();
	BufferVk* vkIndexBuffer = dynamic_cast<BufferVk*>(indexBuffer);
	if(vkIndexBuffer)
	{
		currentCmd.bindIndexBuffer(vkIndexBuffer->BufferHandle(), 0, vk::IndexType::eUint16);
		// #1: index count per instance
		// #2: instance count
		// #3: 
		// #4: ignore first #4 num vertices
		// #5: same #4 but instance
		currentCmd.drawIndexed(6, 1, 0, 0, 0);
	}
	return this;
}

IRHIHandle* VkHandle::Dispatch(Uint32 threadGroupCountX, Uint32 threadGroupCountY, Uint32 threadGroupCountZ)
{
	pComputePending->PrepareDispatch();
	currentCmd.dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
	return this;
}

IRHIHandle* VkHandle::UpdateBuffer(IBuffer *buffer, void *data, Uint32 dataSize, Uint32 offset)
{
	BufferVk* vkBuffer = dynamic_cast<BufferVk*>(buffer);
	if(vkBuffer)
	{
		vkBuffer->UpdateBufferData(data, dataSize, offset);
	}
	return this;
}

IRHIHandle* VkHandle::UpdateImageView(IImageView *imageView, void *data, Uint32 dataSize)
{
	ImageViewVk* vkImageView = dynamic_cast<ImageViewVk*>(imageView);
	if(vkImageView)
	{
		vkImageView->SetImageData(data, dataSize);
	}
	return this;
}

IRHIHandle* VkHandle::CopyBuffer(IBuffer *srcBuffer, IBuffer *dstBuffer)
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
	return this;
}

IRHIHandle* VkHandle::CopyBufferToImage(IBuffer *srcBuffer, IImageView *dstImageView)
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
	return this;
}

IRHIHandle* VkHandle::CopyImageToBuffer(IImageView *srcImageView, IBuffer *dstBuffer)
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
	return this;
}

IRHIHandle* VkHandle::CopyImageToImage(IImageView *srcImageView, IImageView *dstImageView)
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
	return this;
}
