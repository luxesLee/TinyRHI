#pragma once
#include "IRHIHandle.h"
#include "HeaderVk.h"
#include "CommandPoolVk.h"
#include <memory>
#include "PendingStateVk.h"
#include "RenderResourceVkManager.h"

class GLFWwindow;

namespace TinyRHI
{
    class VkHandle : public IRHIHandle
    {
    public:
        explicit VkHandle(GLFWwindow* _window);
        ~VkHandle()
		{
			deviceData.logicalDevice.destroy();
		}
        VkHandle(const VkHandle&) = delete;
        VkHandle& operator=(const VkHandle&) = delete;

	private:
		void InitVulkan();
		void InitInstanceAndPhysicalDevice();
		void InitSurface();
		void InitDevice();
		void InitSwapChain();
		void RecreateSwapChain();
		void InitSync();
		void InitPendingState()
		{
			pGfxPending = std::make_unique<GfxPendingStateVk>();
			pComputePending = std::make_unique<ComputePendingStateVk>();
			pRenderPassBeginManager = std::make_unique<RenderResourceVkManager>(deviceData);
		}

#ifdef DEBUG_VULKAN_MACRO
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessageCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);
#endif

	public:
        // 
        // ------------------------------------------------------------------------------------------------
		
		virtual IShader* CreateVertexShader(const ShaderDesc& shaderDesc);
		virtual IShader* CreatePixelShader(const ShaderDesc& shaderDesc);
		virtual IShader* CreateComputeShader(const ShaderDesc& shaderDesc);

		virtual IBuffer* CreateBuffer(const BufferDesc& bufferDesc);
		virtual IBuffer* CreateBufferWithData(const BufferDesc& bufferDesc, void* data, Uint32 dataSize);
		virtual ITexture* CreateTextureWithoutSampling(const ImageDesc& imageDesc);
		virtual ITexture* CreateTexture(const ImageDesc& imageDesc, const SamplerState& samplerState);
		virtual ITexture* CreateTextureWithData(const ImageDesc& imageDesc, const SamplerState& samplerState, void* data, Uint32 dataSize);

		virtual Uint32 GetTotalVRAM() const;
		virtual Uint32 GetUsedVRAM() const;
    
        // 
        // ------------------------------------------------------------------------------------------------

		virtual IRHIHandle* BeginFrame();
		virtual IRHIHandle* EndFrame();

		virtual IRHIHandle* BeginCommand();
		virtual IRHIHandle* EndCommand();
		virtual IRHIHandle* Commit();

		virtual IRHIHandle* BeginRenderPass();
		virtual IRHIHandle* EndRenderPass();

		virtual IRHIHandle* SetGraphicsPipeline(const GfxSetting& gfxSetting);
		virtual IRHIHandle* SetComputePipeline();

		virtual IRHIHandle* SetDefaultAttachments(const AttachmentDesc &attachmentDesc);
		virtual IRHIHandle* SetColorAttachments(ITexture* texture, const AttachmentDesc& attachmentDesc);
		virtual IRHIHandle* SetDepthAttachment(ITexture* texture, const AttachmentDesc& attachmentDesc);

		virtual IRHIHandle* SetVertexShader(IShader* shader);
		virtual IRHIHandle* SetPixelShader(IShader* shader);
		virtual IRHIHandle* SetComputeShader(IShader* shader);

		virtual IRHIHandle* SetVertexStream(Uint32 vertId, IBuffer* buffer, Uint32 offset);
		virtual IRHIHandle* SetViewport(Extent3D minExt, Extent3D maxExt);
		virtual IRHIHandle* SetScissor(Extent2D minExt, Extent2D maxExt);

		virtual IRHIHandle* SetSamplerTexture(ITexture* texture, Uint setId, Uint bindingId);
		virtual IRHIHandle* SetStorageTexture(ITexture* texture, Uint setId, Uint bindingId);
		virtual IRHIHandle* SetStorageBuffer(IBuffer* buffer, Uint setId, Uint bindingId);
		virtual IRHIHandle* SetUniformBuffer(IBuffer* Buffer, Uint setId, Uint bindingId);

        virtual IRHIHandle* DrawPrimitive(Uint32 vertexCount, Uint32 firstVertex);
		virtual IRHIHandle* DrawPrimitiveIndirect(IBuffer* argumentBuffer, Uint32 argumentOffset);
		virtual IRHIHandle* DrawIndexPrimitive(IBuffer* indexBuffer, Int32 baseVertexIndex, Uint32 firstInstance, Uint32 startIndex, Uint32 numPrimitives, Uint32 numInstances);
		virtual IRHIHandle* Dispatch(Uint32 threadGroupCountX, Uint32 threadGroupCountY, Uint32 threadGroupCountZ);

		virtual IRHIHandle* UpdateBuffer(IBuffer* buffer, void* data, Uint32 dataSize, Uint32 offset);
		virtual IRHIHandle* UpdateImageView(IImageView* imageView, void* data, Uint32 dataSize);
        virtual IRHIHandle* CopyBuffer(IBuffer* srcBuffer, IBuffer* dstBuffer);
		virtual IRHIHandle* CopyBufferToImage(IBuffer* srcBuffer, IImageView* dstImageView);
		virtual IRHIHandle* CopyImageToBuffer(IImageView* srcImageView, IBuffer* dstBuffer);
		virtual IRHIHandle* CopyImageToImage(IImageView* srcImageView, IImageView* dstImageView);



    private:
		GLFWwindow* window;

		vk::UniqueInstance instance;

#ifdef DEBUG_VULKAN_MACRO
		vk::DispatchLoaderDynamic loader;
    	vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> debugUtilsMessenger;
#endif

		std::vector<vk::PhysicalDevice> physicalDevices;
		DeviceData deviceData;

    	VkSurfaceKHR surface;
		vk::UniqueSwapchainKHR swapChain;
		vk::Extent2D swapChainExtent;
		std::vector<std::unique_ptr<ImageViewVk>> swapImageViews;
		Uint32 swapImageIndex;

		std::vector<vk::UniqueSemaphore> swapImageAvailableSemaphores;
		std::vector<vk::UniqueSemaphore> renderFinishedSemaphores;
		std::vector<vk::UniqueFence> inFlightFences;
		Uint currentFrame;

		std::unique_ptr<CommandPoolManager> cmdPoolManager;

		vk::CommandBuffer currentCmd;

		Bool bCurrentGfx;
		std::unique_ptr<GfxPendingStateVk> pGfxPending;
		std::unique_ptr<ComputePendingStateVk> pComputePending;

		std::unique_ptr<RenderResourceVkManager> pRenderPassBeginManager;
    };


} // namespace TinyRHI
