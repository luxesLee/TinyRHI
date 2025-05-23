#pragma once
#include "IRHIHandle.h"

class GLFWwindow;

namespace TinyRHI
{
    class HandleOgl : public IRHIHandle
    {
    public:
        explicit HandleOgl(GLFWwindow* _window)
		{
			InitOpenGL();
		}
        ~HandleOgl()
		{
		}
        HandleOgl(const HandleOgl&) = delete;
        HandleOgl& operator=(const HandleOgl&) = delete;

	private:
		void InitOpenGL();

	public:

        // 
        // ------------------------------------------------------------------------------------------------
        virtual IGraphicsPipeline* CreateGrpahicsPipeline(const GraphicsPipelineDesc& gfxPipelineDesc);
		virtual IComputePipeline* CreateComputePipeline(const ComputePipelineDesc& computePipelineDesc);

		virtual IShader* CreateVertexShader(const ShaderDesc& shaderDesc);
		virtual IShader* CreatePixelShader(const ShaderDesc& shaderDesc);
		virtual IShader* CreateComputeShader(const ShaderDesc& shaderDesc);

		virtual IBuffer* CreateBuffer(const BufferDesc& bufferDesc) = 0;
		virtual IImageView* CreateImageView(const ImageDesc& imageDesc) = 0;
		virtual IImageView* CreateImageViewWithData(const ImageDesc& imageDesc) = 0;

		virtual IFramebuffer* CreateFrameBuffer(const FramebufferDesc& framebufferDesc) = 0;

		virtual Uint32 GetTotalVRAM() const = 0;
		virtual Uint32 GetUsedVRAM() const = 0;
    
        // 
        // ------------------------------------------------------------------------------------------------

		virtual IRHIHandle* BeginRenderPass() = 0;
		virtual IRHIHandle* EndRenderPass() = 0;

		virtual IRHIHandle* SetGraphicsPipelineState() = 0;
		virtual IRHIHandle* SetComputePipelineState() = 0;

		virtual IRHIHandle* SetVertexStream() = 0;
		virtual IRHIHandle* SetViewport(Extent3D minExt, Extent3D maxExt) = 0;
		virtual IRHIHandle* SetScissor(Bool bEnable, Extent2D minExt, Extent2D maxExt) = 0;

		virtual IRHIHandle* SetSamplerTexture(ITexture* texture, Uint setId, Uint bindingId);
		virtual IRHIHandle* SetStorageTexture(ITexture* texture, Uint setId, Uint bindingId);
		virtual IRHIHandle* SetStorageBuffer(IBuffer* buffer, Uint setId, Uint bindingId);
		virtual IRHIHandle* SetUniformBuffer(IBuffer* Buffer, Uint setId, Uint bindingId);

        virtual IRHIHandle* DrawPrimitive(Uint32 baseVertexIndex, Uint32 numPrimitives, Uint32 numInstances);
		virtual IRHIHandle* DrawPrimitiveIndirect(IBuffer* argumentBuffer, Uint32 argumentOffset);
		virtual IRHIHandle* DrawIndexPrimitive(IBuffer* indexBuffer, Int32 baseVertexIndex, Uint32 firstInstance, Uint32 startIndex, Uint32 numPrimitives, Uint32 numInstances);
		virtual IRHIHandle* Dispatch(Uint32 threadGroupCountX, Uint32 threadGroupCountY, Uint32 threadGroupCountZ);

        virtual IRHIHandle* CopyBuffer(IBuffer* srcBuffer, IBuffer* dstBuffer) = 0;
		virtual IRHIHandle* CopyBufferToImage(IBuffer* srcBuffer, IImageView* dstImageView) = 0;
		virtual IRHIHandle* CopyImageToBuffer(IImageView* srcImageView, IBuffer* dstBuffer) = 0;
		virtual IRHIHandle* CopyImageToImage(IImageView* srcImageView, IImageView* dstImageView) = 0;

        virtual IRHIHandle* Commit() = 0;
    };
}