#pragma once
#include "IRHIHandle.h"
#include "HeaderVk.h"
#include "CommandPoolVk.h"
#include <memory>


namespace TinyRHI
{
	class GfxPendingStateVk;
	class ComputePendingStateVk;

    class VkHandle : public IRHIHandle
    {
    public:
        VkHandle()
		{
			InitVulkan();
		}
        ~VkHandle()
		{
			deviceData.logicalDevice.destroy();
		}
        VkHandle(const VkHandle&) = delete;
        VkHandle& operator=(const VkHandle&) = delete;

	private:
		void InitVulkan();
		void InitPendingState()
		{

		}

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
		virtual IImageView* CreateImageView(const ImageDesc& imageDesc);
		virtual IImageView* CreateImageViewWithData(const ImageDesc& imageDesc, void* data, Uint32 dataSize);

		virtual Uint32 GetTotalVRAM() const;
		virtual Uint32 GetUsedVRAM() const;
    
        // 
        // ------------------------------------------------------------------------------------------------

		// ExcuteOrder
		// BeginFrame -> (BeginCommand -> (BeginRenderPass -> EndRenderPass) * m -> EndCommand) * n -> EndFrame
		
		// 这里应该等待fence之类的
		virtual void BeginFrame();
		virtual void EndFrame();

		// 设置新commandBuffer
		virtual void BeginCommand();
		virtual void EndCommand();

		// framebuffer、renderpass设置
		virtual void BeginRenderPass();
		virtual void EndRenderPass();

		virtual void Commit();

		virtual void SetGraphicsPipelineState(IGraphicsPipeline* gfxPipeline);
		virtual void SetComputePipelineState(IComputePipeline* computePipeline);

		virtual void SetVertexStream(IBuffer* buffer, Uint32 offset);
		virtual void SetViewport(Extent3D minExt, Extent3D maxExt);
		virtual void SetScissor(Extent2D minExt, Extent2D maxExt);

		virtual void SetShaderTexture(IShader* shader) = 0;
		virtual void SetShaderUniformBuffer(IShader::Stage stage, IBuffer* uniformBuffer, Int32 bufferIndex, IShader* shader) = 0;

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
		vk::UniqueInstance instance;
		std::vector<vk::PhysicalDevice> physicalDevices;

		DeviceData deviceData;

		std::unique_ptr<CommandPoolManager> cmdPoolManager;

		vk::CommandBuffer currentCmd;

		std::unique_ptr<GfxPendingStateVk> pGfxPending;
		std::unique_ptr<ComputePendingStateVk> pComputePending;
    };


} // namespace TinyRHI
