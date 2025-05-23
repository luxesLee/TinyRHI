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

		virtual IShader* CreateVertexShader(ShaderDesc& shaderDesc);
		virtual IShader* CreatePixelShader(ShaderDesc& shaderDesc);
		virtual IShader* CreateComputeShader(ShaderDesc& shaderDesc);

		virtual IBuffer* CreateBuffer(const BufferDesc& bufferDesc) = 0;
		virtual IImageView* CreateImageView(const ImageDesc& imageDesc) = 0;
		virtual IImageView* CreateImageViewWithData(const ImageDesc& imageDesc) = 0;

		virtual IFramebuffer* CreateFrameBuffer(const FramebufferDesc& framebufferDesc) = 0;

		virtual Uint32 GetTotalVRAM() const = 0;
		virtual Uint32 GetUsedVRAM() const = 0;
    
        // 
        // ------------------------------------------------------------------------------------------------

		virtual void BeginRenderPass() = 0;
		virtual void EndRenderPass() = 0;

		virtual void SetGraphicsPipelineState() = 0;
		virtual void SetComputePipelineState() = 0;

		virtual void SetVertexStream() = 0;
		virtual void SetViewport(Extent3D minExt, Extent3D maxExt) = 0;
		virtual void SetScissor(Bool bEnable, Extent2D minExt, Extent2D maxExt) = 0;

		virtual void SetSamplerTexture(ITexture* texture, Uint setId, Uint bindingId);
		virtual void SetStorageTexture(ITexture* texture, Uint setId, Uint bindingId);
		virtual void SetStorageBuffer(IBuffer* buffer, Uint setId, Uint bindingId);
		virtual void SetUniformBuffer(IBuffer* Buffer, Uint setId, Uint bindingId);

        virtual void DrawPrimitive(Uint32 baseVertexIndex, Uint32 numPrimitives, Uint32 numInstances);
		virtual void DrawPrimitiveIndirect(IBuffer* argumentBuffer, Uint32 argumentOffset);
		virtual void DrawIndexPrimitive(IBuffer* indexBuffer, Int32 baseVertexIndex, Uint32 firstInstance, Uint32 startIndex, Uint32 numPrimitives, Uint32 numInstances);
		virtual void Dispatch(Uint32 threadGroupCountX, Uint32 threadGroupCountY, Uint32 threadGroupCountZ);

        virtual void CopyBuffer(IBuffer* srcBuffer, IBuffer* dstBuffer) = 0;
		virtual void CopyBufferToImage(IBuffer* srcBuffer, IImageView* dstImageView) = 0;
		virtual void CopyImageToBuffer(IImageView* srcImageView, IBuffer* dstBuffer) = 0;
		virtual void CopyImageToImage(IImageView* srcImageView, IImageView* dstImageView) = 0;

        virtual void Commit() = 0;
    };
}