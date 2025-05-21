#pragma once
#include "BaseType.h"

#include "IShader.h"
#include "IBuffer.h"
#include "IImageView.h"
#include "ISampler.h"
#include "IRenderPass.h"
#include "IPipeline.h"
#include "IFramebuffer.h"

namespace TinyRHI
{
    class IRHIHandle
    {
    public:

        // 
        // ------------------------------------------------------------------------------------------------

		virtual IGraphicsPipeline* CreateGrpahicsPipeline(const GraphicsPipelineDesc& gfxPipelineDesc) = 0;
		virtual IComputePipeline* CreateComputePipeline(const ComputePipelineDesc& computePipelineDesc) = 0;

		virtual IShader* CreateVertexShader(ShaderDesc& shaderDesc) = 0;
		virtual IShader* CreatePixelShader(ShaderDesc& shaderDesc) = 0;
		virtual IShader* CreateComputeShader(ShaderDesc& shaderDesc) = 0;

		virtual IBuffer* CreateBuffer(const BufferDesc& bufferDesc) = 0;
		virtual IImageView* CreateImageView(const ImageDesc& imageDesc) = 0;
		virtual IImageView* CreateImageViewWithData(const ImageDesc& imageDesc) = 0;

		virtual IFramebuffer* CreateFrameBuffer(const FramebufferDesc& framebufferDesc) = 0;

		virtual Uint32 GetTotalVRAM() const = 0;
		virtual Uint32 GetUsedVRAM() const = 0;

        // Cmd
        // ------------------------------------------------------------------------------------------------

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void BeginCommand() = 0;
		virtual void EndCommand() = 0;

		virtual void BeginRenderPass() = 0;
		virtual void EndRenderPass() = 0;

		virtual void Commit() = 0;

		virtual void SetGraphicsPipelineState(IGraphicsPipeline* gfxPipeline) = 0;
		virtual void SetComputePipelineState(IComputePipeline* computePipeline) = 0;

		virtual void SetVertexStream(IBuffer* buffer, Uint32 offset) = 0;
		virtual void SetViewport(Extent3D minExt, Extent3D maxExt) = 0;
		virtual void SetScissor(Extent2D minExt, Extent2D maxExt) = 0;

		virtual void SetTexture(IShader* shader) = 0;
		virtual void SetBuffer(IShader::Stage stage, IBuffer* buffer, Uint32 bufferIndex) = 0;
		virtual void SetUniformBuffer(IShader::Stage stage, IBuffer* uniformBuffer, Int32 bufferIndex, IShader* shader) = 0;

		virtual void DrawPrimitive(Uint32 baseVertexIndex, Uint32 numPrimitives, Uint32 numInstances) = 0;
		virtual void DrawPrimitiveIndirect(IBuffer* argumentBuffer, Uint32 argumentOffset) = 0;
		virtual void DrawIndexPrimitive(IBuffer* indexBuffer, Int32 baseVertexIndex, Uint32 firstInstance, Uint32 startIndex, Uint32 numPrimitives, Uint32 numInstances) = 0;
		virtual void Dispatch(Uint32 threadGroupCountX, Uint32 threadGroupCountY, Uint32 threadGroupCountZ) = 0;

		virtual void UpdateBuffer(IBuffer* buffer, void* data, Uint32 dataSize, Uint32 offset) = 0;
		virtual void UpdateImageView(IImageView* imageView, void* data, Uint32 dataSize) = 0;
		virtual void CopyBuffer(IBuffer* srcBuffer, IBuffer* dstBuffer) = 0;
		virtual void CopyBufferToImage(IBuffer* srcBuffer, IImageView* dstImageView) = 0;
		virtual void CopyImageToBuffer(IImageView* srcImageView, IBuffer* dstBuffer) = 0;
		virtual void CopyImageToImage(IImageView* srcImageView, IImageView* dstImageView) = 0;

    };
}