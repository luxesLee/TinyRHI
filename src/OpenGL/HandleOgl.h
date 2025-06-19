#pragma once
#ifdef RHI_SUPPORT_OPENGL

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

		virtual IShader* CreateVertexShader(const ShaderDesc& shaderDesc);
		virtual IShader* CreatePixelShader(const ShaderDesc& shaderDesc);
		virtual IShader* CreateComputeShader(const ShaderDesc& shaderDesc);

		virtual IBuffer* CreateBuffer(const BufferDesc& bufferDesc) = 0;
		virtual IBuffer* CreateBufferWithData(const BufferDesc& bufferDesc, void* data, Uint32 dataSize) = 0;
		virtual ITexture* CreateTextureWithoutSampling(const ImageDesc& imageDesc) = 0;
		virtual ITexture* CreateTexture(const ImageDesc& imageDesc, const SamplerState& samplerState) = 0;
		virtual ITexture* CreateTextureWithData(const ImageDesc& imageDesc, const SamplerState& samplerState, void* data, Uint32 dataSize) = 0;

		virtual Uint32 GetTotalVRAM() const;
		virtual Uint32 GetUsedVRAM() const;
    
        // 
        // ------------------------------------------------------------------------------------------------

		virtual IRHIHandle* BeginFrame();
		virtual IRHIHandle* EndFrame();

		virtual IRHIHandle* BeginCommand() = 0;
		virtual IRHIHandle* EndCommand() = 0;

		virtual IRHIHandle* BeginRenderPass() = 0;
		virtual IRHIHandle* EndRenderPass() = 0;

		virtual IRHIHandle* Commit() = 0;

		virtual IRHIHandle* SetGraphicsPipeline(const GfxSetting& gfxSetting) = 0;
		virtual IRHIHandle* SetComputePipeline() = 0;

		virtual IRHIHandle* SetTransition(ITransition* trans) = 0;

		virtual IRHIHandle* SetDefaultAttachments(const AttachmentDesc &attachmentDesc) = 0;
		virtual IRHIHandle* SetColorAttachments(ITexture* texture, const AttachmentDesc& attachmentDesc) = 0;
		virtual IRHIHandle* SetDepthAttachment(ITexture* texture, const AttachmentDesc& attachmentDesc) = 0;

		virtual IRHIHandle* SetVertexShader(IShader* shader) = 0;
		virtual IRHIHandle* SetPixelShader(IShader* shader) = 0;
		virtual IRHIHandle* SetComputeShader(IShader* shader) = 0;

		virtual IRHIHandle* SetVertexStream(Uint32 vertId, IBuffer* buffer, Uint32 offset) = 0;
		virtual IRHIHandle* SetViewport(Extent2D minExt, Extent2D maxExt);
		virtual IRHIHandle* SetViewport(Extent3D minExt, Extent3D maxExt);
		virtual IRHIHandle* SetScissor(Extent2D minExt, Extent2D maxExt);

		virtual IRHIHandle* SetSamplerTexture(ITexture* texture, IShader::Stage stage, Uint setId, Uint bindingId) = 0;
		virtual IRHIHandle* SetStorageTexture(ITexture* texture, IShader::Stage stage, Uint setId, Uint bindingId) = 0;
		virtual IRHIHandle* SetStorageBuffer(IBuffer* buffer, IShader::Stage stage, Uint setId, Uint bindingId) = 0;
		virtual IRHIHandle* SetUniformBuffer(IBuffer* Buffer, IShader::Stage stage, Uint setId, Uint bindingId) = 0;

		virtual IRHIHandle* DrawPrimitive(Uint32 vertexCount, Uint32 firstVertex) = 0;
		virtual IRHIHandle* DrawPrimitiveIndirect(IBuffer* argumentBuffer, Uint32 argumentOffset) = 0;
		virtual IRHIHandle* DrawIndexPrimitive(IBuffer *indexBuffer, Uint32 indexCount, Uint32 firstIndex, Int32 vertOffset) = 0;
		virtual IRHIHandle* Dispatch(Uint32 threadGroupCountX, Uint32 threadGroupCountY, Uint32 threadGroupCountZ) = 0;

		virtual IRHIHandle* UpdateBuffer(IBuffer* buffer, void* data, Uint32 dataSize, Uint32 offset) = 0;
		virtual IRHIHandle* UpdateImageView(IImageView* imageView, void* data, Uint32 dataSize) = 0;
		virtual IRHIHandle* CopyBuffer(IBuffer* srcBuffer, IBuffer* dstBuffer) = 0;
		virtual IRHIHandle* CopyBufferToImage(IBuffer* srcBuffer, IImageView* dstImageView) = 0;
		virtual IRHIHandle* CopyImageToBuffer(IImageView* srcImageView, IBuffer* dstBuffer) = 0;
		virtual IRHIHandle* CopyImageToImage(IImageView* srcImageView, IImageView* dstImageView) = 0;
    };
}

#endif
