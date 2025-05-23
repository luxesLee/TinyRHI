#pragma once
#include "IRHIHandle.h"
#include "HeaderVk.h"
#include "CommandPoolVk.h"
#include <memory>
#include "PendingStateVk.h"

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
		void InitPendingState()
		{
			pGfxPending = std::make_unique<GfxPendingStateVk>();
			pComputePending = std::make_unique<ComputePendingStateVk>();
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
		
        virtual IGraphicsPipeline* CreateGrpahicsPipeline(const GraphicsPipelineDesc& gfxPipelineDesc);
		virtual IComputePipeline* CreateComputePipeline(const ComputePipelineDesc& computePipelineDesc);

		virtual IShader* CreateVertexShader(ShaderDesc& shaderDesc);
		virtual IShader* CreatePixelShader(ShaderDesc& shaderDesc);
		virtual IShader* CreateComputeShader(ShaderDesc& shaderDesc);

		virtual IBuffer* CreateBuffer(const BufferDesc& bufferDesc);
		virtual IBuffer* CreateBufferWithData(const BufferDesc& bufferDesc, void* data, Uint32 dataSize);
		virtual ITexture* CreateTexture(const ImageDesc& imageDesc, const SamplerState& samplerState);
		virtual ITexture* CreateTextureWithData(const ImageDesc& imageDesc, const SamplerState& samplerState, void* data, Uint32 dataSize);

		virtual Uint32 GetTotalVRAM() const;
		virtual Uint32 GetUsedVRAM() const;
    
        // 
        // ------------------------------------------------------------------------------------------------

		// ExcuteOrder
		// BeginFrame -> 
		// 	(BeginCommand -> 
		// 		(BeginRenderPass -> setPipeline -> setxxx -> draw -> EndRenderPass) * m -> 
		// 	EndCommand) * n -> 
		// EndFrame
		
		virtual void BeginFrame();
		virtual void EndFrame();

		virtual void BeginCommand();
		virtual void EndCommand();

		virtual void BeginRenderPass();
		virtual void EndRenderPass();

		virtual void Commit();

		virtual void SetGraphicsPipelineState(IGraphicsPipeline* gfxPipeline);
		virtual void SetComputePipelineState(IComputePipeline* computePipeline);

		virtual void SetVertexStream(Uint32 vertId, IBuffer* buffer, Uint32 offset);
		virtual void SetViewport(Extent3D minExt, Extent3D maxExt);
		virtual void SetScissor(Extent2D minExt, Extent2D maxExt);

		virtual void SetSamplerTexture(ITexture* texture, Uint setId, Uint bindingId);
		virtual void SetStorageTexture(ITexture* texture, Uint setId, Uint bindingId);
		virtual void SetStorageBuffer(IBuffer* buffer, Uint setId, Uint bindingId);
		virtual void SetUniformBuffer(IBuffer* Buffer, Uint setId, Uint bindingId);

        virtual void DrawPrimitive(Uint32 baseVertexIndex, Uint32 numPrimitives, Uint32 numInstances);
		virtual void DrawPrimitiveIndirect(IBuffer* argumentBuffer, Uint32 argumentOffset);
		virtual void DrawIndexPrimitive(IBuffer* indexBuffer, Int32 baseVertexIndex, Uint32 firstInstance, Uint32 startIndex, Uint32 numPrimitives, Uint32 numInstances);
		virtual void Dispatch(Uint32 threadGroupCountX, Uint32 threadGroupCountY, Uint32 threadGroupCountZ);

		virtual void UpdateBuffer(IBuffer* buffer, void* data, Uint32 dataSize, Uint32 offset);
		virtual void UpdateImageView(IImageView* imageView, void* data, Uint32 dataSize);
        virtual void CopyBuffer(IBuffer* srcBuffer, IBuffer* dstBuffer);
		virtual void CopyBufferToImage(IBuffer* srcBuffer, IImageView* dstImageView);
		virtual void CopyImageToBuffer(IImageView* srcImageView, IBuffer* dstBuffer);
		virtual void CopyImageToImage(IImageView* srcImageView, IImageView* dstImageView);



    private:
		GLFWwindow* window;

		vk::UniqueInstance instance;

#ifdef DEBUG_VULKAN_MACRO
		vk::DispatchLoaderDynamic loader;
    	vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> debugUtilsMessenger;
#endif

		std::vector<vk::PhysicalDevice> physicalDevices;

    	VkSurfaceKHR surface;
		vk::UniqueSwapchainKHR swapChain;
		std::vector<std::unique_ptr<ImageViewVk>> swapImageViews;
		Uint32 swapImageIndex;

		DeviceData deviceData;

		std::unique_ptr<CommandPoolManager> cmdPoolManager;

		vk::CommandBuffer currentCmd;

		Bool bCurrentGfx;
		std::unique_ptr<GfxPendingStateVk> pGfxPending;
		std::unique_ptr<ComputePendingStateVk> pComputePending;
    };


} // namespace TinyRHI
