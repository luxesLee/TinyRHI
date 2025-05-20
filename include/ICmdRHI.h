#pragma once
#include "BaseType.h"

#include "IShader.h"
#include "IBuffer.h"
#include "IImageView.h"

namespace TinyRHI
{
	class ICmdRHI
	{
	public:

		//virtual void BeginRenderPass(RenderPass* renderPass, FrameBuffer* framebuffer) = 0;
		//virtual void EndRenderPass() = 0;


		virtual void SetGraphicsPipelineState() = 0;
		virtual void SetComputePipelineState() = 0;

		virtual void SetStream() = 0;
		virtual void SetViewport(Extent3D minExt, Extent3D maxExt) = 0;
		virtual void SetScissor(Bool bEnable, Extent2D minExt, Extent2D maxExt) = 0;

		virtual void SetComputeShader() = 0;
		virtual void SetShaderTexture(IShader* shader) = 0;
		virtual void SetShaderUniformBuffer(IShader::Stage stage, IBuffer* uniformBuffer, Int32 bufferIndex, IShader* shader) = 0;

		virtual void DrawPrimitive(Uint32 baseVertexIndex, Uint32 numPrimitives, Uint32 numInstances) = 0;
		virtual void DrawPrimitiveIndirect(IBuffer* argumentBuffer, Uint32 argumentOffset) = 0;
		virtual void DrawIndexPrimitive(IBuffer* indexBuffer, Int32 baseVertexIndex, Uint32 firstInstance, Uint32 startIndex, Uint32 numPrimitives, Uint32 numInstances) = 0;
		virtual void Dispatch(Uint32 threadGroupCountX, Uint32 threadGroupCountY, Uint32 threadGroupCountZ) = 0;

		virtual void CopyBuffer(IBuffer* srcBuffer, IBuffer* dstBuffer) = 0;
		virtual void CopyBufferToImage(IBuffer* srcBuffer, IImageView* dstImageView) = 0;
		virtual void CopyImageToBuffer(IImageView* srcImageView, IBuffer* dstBuffer) = 0;
		virtual void CopyImageToImage(IImageView* srcImageView, IImageView* dstImageView) = 0;

		virtual void Commit() = 0;
	};
}