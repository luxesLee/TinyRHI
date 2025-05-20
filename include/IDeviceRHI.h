#pragma once
#include "BaseType.h"

#include "IShader.h"
#include "IBuffer.h"
#include "IImageView.h"
#include "IFramebuffer.h"
#include "IPipeline.h"

namespace TinyRHI
{
	class IDeviceRHI
	{
	public:

		virtual IGraphicsPipeline* CreateGrpahicsPipeline(const GraphicsPipelineDesc& renderPipelineDesc) = 0;
		virtual IComputePipeline* CreateComputePipeline(const ComputePipelineDesc& computePipelineDesc) = 0;

		virtual IShader* CreateVertexShader(Uint32* data, Uint32 dataSize) = 0;
		virtual IShader* CreatePixelShader(Uint32* data, Uint32 dataSize) = 0;
		virtual IShader* CreateGeometryShader(Uint32* data, Uint32 dataSize) = 0;
		virtual IShader* CreateComputeShader(Uint32* data, Uint32 dataSize) = 0;

		virtual IBuffer* CreateBuffer(const BufferDesc& bufferDesc) = 0;
		virtual IImageView* CreateImageView(const ImageDesc& imageDesc) = 0;
		virtual IImageView* CreateImageViewWithData(const ImageDesc& imageDesc) = 0;

		virtual IFramebuffer* CreateFrameBuffer(const FramebufferDesc& framebufferDesc) = 0;
		
		virtual Uint32 GetTotalVRAM() const = 0;
		virtual Uint32 GetUsedVRAM() const = 0;
	};
}