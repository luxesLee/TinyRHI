#pragma once
#include <vulkan/vulkan.hpp>
#include "IBuffer.h"
#include "IFramebuffer.h"
#include "IImageView.h"
#include "IPipeline.h"
#include "IRenderPass.h"
#include "ISampler.h"
#include "IShader.h"

namespace TinyRHI
{
	struct DeviceData
	{
		struct
		{
			vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;
			vk::Device logicalDevice = VK_NULL_HANDLE;

			vk::Queue graphicsQueue = VK_NULL_HANDLE;
			vk::Queue presentQueue = VK_NULL_HANDLE;

			struct QueueFamilyIndices
			{
				uint32_t graphicsFamilyIndex;
				uint32_t presentFamilyIndex;
			} queueFamilyIndices;

			vk::CommandPool commandPool = VK_NULL_HANDLE;
		};
	};

	inline vk::CommandBuffer BeginSingleTimeCommands(const DeviceData& deviceData)
	{
		auto cmdBufferAllocInfo = vk::CommandBufferAllocateInfo()
			.setLevel(vk::CommandBufferLevel::ePrimary)	// ��Ҫ�������������ֱ���ύ����
			.setCommandPool(deviceData.commandPool)
			.setCommandBufferCount(1);
		vk::CommandBuffer cmdBuffer = (deviceData.logicalDevice.allocateCommandBuffers(cmdBufferAllocInfo))[0];

		auto beginInfo = vk::CommandBufferBeginInfo()
			.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);	// ��һ���ύʹ��
		cmdBuffer.begin(beginInfo);

		return cmdBuffer;
	};

	inline void EndSingleTimeCommands(const DeviceData& deviceData, vk::CommandBuffer cmdBuffer)
	{
		cmdBuffer.end();

		auto submitInfo = vk::SubmitInfo()
			.setCommandBufferCount(1)
			.setPCommandBuffers(&cmdBuffer);
		deviceData.graphicsQueue.submit({ submitInfo });
		deviceData.graphicsQueue.waitIdle();

		deviceData.logicalDevice.freeCommandBuffers(deviceData.commandPool, { cmdBuffer });
	};

	inline void TransitionImageLayout(const DeviceData& deviceData, vk::Image image, const ImageDesc& imageDesc, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
	{
		vk::CommandBuffer cmdBuffer = BeginSingleTimeCommands(deviceData);

		auto subresourceRange = vk::ImageSubresourceRange()
			.setAspectMask(imageDesc.bDepth ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor)
			.setBaseArrayLayer(imageDesc.imageViewDesc.baseArrayLayer)
			.setLayerCount(imageDesc.imageViewDesc.arrayLayersCount)
			.setBaseMipLevel(imageDesc.imageViewDesc.baseMipLevel)
			.setLevelCount(imageDesc.imageViewDesc.mipLevelsCount);

		auto barrier = vk::ImageMemoryBarrier()
			.setOldLayout(oldLayout)
			.setNewLayout(newLayout)
			.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setImage(image)
			.setSubresourceRange(subresourceRange);

		vk::PipelineStageFlags srcStage, dstStage;
		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
		{
			barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
			barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

			srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
			dstStage = vk::PipelineStageFlagBits::eTransfer;
		}
		else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
		{
			barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
			barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

			srcStage = vk::PipelineStageFlagBits::eTransfer;
			dstStage = vk::PipelineStageFlagBits::eBottomOfPipe;
		}
		else
		{
			throw std::invalid_argument("unsupported layout transition!");
		}

		cmdBuffer.pipelineBarrier(srcStage, dstStage, vk::DependencyFlags(), {}, {}, {barrier});

		EndSingleTimeCommands(deviceData, cmdBuffer);
	}

	inline Uint32 findMemoryType(vk::PhysicalDeviceMemoryProperties availableMemProperties, vk::MemoryPropertyFlags memProp, Uint32 typeFilter)
	{
		for (Uint32 i = 0; i < availableMemProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i))
				&& (availableMemProperties.memoryTypes[i].propertyFlags & memProp) == memProp)
			{
				return i;
			}
		}
		return 0;
	}

	inline vk::BufferUsageFlags ConvertBufferUsage(BufferType bufferType)
	{
		vk::BufferUsageFlags usage;
		if (bufferType.bVertex)
		{
			usage |= vk::BufferUsageFlagBits::eVertexBuffer;
		}
		if (bufferType.bIndex)
		{
			usage |= vk::BufferUsageFlagBits::eIndexBuffer;
		}
		if (bufferType.bStorage)
		{
			usage |= vk::BufferUsageFlagBits::eStorageBuffer;
		}
		if (bufferType.bUniform)
		{
			usage |= vk::BufferUsageFlagBits::eUniformBuffer;
		}
		if (bufferType.bIndirect)
		{
			usage |= vk::BufferUsageFlagBits::eIndirectBuffer;
		}
		return usage;
	}

	inline vk::Format ConvertAttribType(AttribType attribType)
	{
		switch (attribType)
		{
		case AttribType::Vec1:
			return vk::Format::eR32Sfloat;
		case AttribType::Vec2:
			return vk::Format::eR32G32Sfloat;
		case AttribType::Vec3:
			return vk::Format::eR32G32B32Sfloat;
		case AttribType::Vec4:
			return vk::Format::eR32G32B32A32Sfloat;
		case AttribType::Vec3_Unorm:
			return vk::Format::eR8G8B8Unorm;
		case AttribType::Vec4_Unorm:
			return vk::Format::eR8G8B8A8Unorm;
		case AttribType::Color32:
			return vk::Format::eR8G8B8A8Unorm;
		}
		return vk::Format::eUndefined;
	}

	inline vk::PrimitiveTopology ConvertPrimitiveTopology(InputAssemblyState::PrimitiveTopology primitiveTopology)
	{
		switch (primitiveTopology)
		{
		case InputAssemblyState::PrimitiveTopology::PointList:
			return vk::PrimitiveTopology::ePointList;
		case InputAssemblyState::PrimitiveTopology::LineList:
			return vk::PrimitiveTopology::eLineList;
		case InputAssemblyState::PrimitiveTopology::LineStrip:
			return vk::PrimitiveTopology::eLineStrip;
		case InputAssemblyState::PrimitiveTopology::TriangleList:
			return vk::PrimitiveTopology::eTriangleList;
		case InputAssemblyState::PrimitiveTopology::TriangleStrip:
			return vk::PrimitiveTopology::eTriangleStrip;
		case InputAssemblyState::PrimitiveTopology::TriangleFan:
			return vk::PrimitiveTopology::eTriangleFan;
		case InputAssemblyState::PrimitiveTopology::LineListAdjacency:
			return vk::PrimitiveTopology::eLineListWithAdjacency;
		case InputAssemblyState::PrimitiveTopology::LineStripAdjacency:
			return vk::PrimitiveTopology::eLineStripWithAdjacency;
		case InputAssemblyState::PrimitiveTopology::TriangleListAdjacency:
			return vk::PrimitiveTopology::eTriangleListWithAdjacency;
		case InputAssemblyState::PrimitiveTopology::TriangleStripAdjacency:
			return vk::PrimitiveTopology::eTriangleStripWithAdjacency;
		case InputAssemblyState::PrimitiveTopology::PatchList:
			return vk::PrimitiveTopology::ePatchList;
		}
		return vk::PrimitiveTopology::ePointList;
	}

	inline vk::PolygonMode ConvertPolygonMode(RasterizeState::PolygonMode polygonMode)
	{
		switch (polygonMode)
		{
		case RasterizeState::PolygonMode::Fill:
			return vk::PolygonMode::eFill;
		case RasterizeState::PolygonMode::Line:
			return vk::PolygonMode::eLine;
		case RasterizeState::PolygonMode::Point:
			return vk::PolygonMode::ePoint;
		}
		return vk::PolygonMode::eFill;
	}

	inline vk::CullModeFlagBits ConvertCullMode(RasterizeState::CullMode cullMode)
	{
		switch (cullMode)
		{
		case RasterizeState::CullMode::None:
			return vk::CullModeFlagBits::eNone;
		case RasterizeState::CullMode::Front:
			return vk::CullModeFlagBits::eFront;
		case RasterizeState::CullMode::Back:
			return vk::CullModeFlagBits::eBack;
		}
		return vk::CullModeFlagBits::eNone;
	}

	inline vk::FrontFace ConvertFrontFace(RasterizeState::FrontFace frontFace)
	{
		switch (frontFace)
		{
		case RasterizeState::FrontFace::cw:
			return vk::FrontFace::eClockwise;
		case RasterizeState::FrontFace::ccw:
			return vk::FrontFace::eCounterClockwise;
		}
		return vk::FrontFace::eClockwise;
	}

	inline vk::SampleCountFlagBits ConvertMSAASamples(MSAASamples msaaSamples)
	{
		switch (msaaSamples)
		{
		case MSAASamples::e1:
			return vk::SampleCountFlagBits::e1;
		case MSAASamples::e2:
			return vk::SampleCountFlagBits::e2;
		case MSAASamples::e4:
			return vk::SampleCountFlagBits::e4;
		case MSAASamples::e8:
			return vk::SampleCountFlagBits::e8;
		case MSAASamples::e16:
			return vk::SampleCountFlagBits::e16;
		}
		return vk::SampleCountFlagBits::e1;
	}

	inline vk::PipelineColorBlendAttachmentState ConvertBlendState(BlendSetting blendSetting)
	{
		static vk::PipelineColorBlendAttachmentState OpaqueState
			= vk::PipelineColorBlendAttachmentState()
			.setColorWriteMask(vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB)
			.setBlendEnable(false);

		static vk::PipelineColorBlendAttachmentState AddState
			= vk::PipelineColorBlendAttachmentState()
			.setBlendEnable(true)
			.setAlphaBlendOp(vk::BlendOp::eAdd)
			.setColorBlendOp(vk::BlendOp::eAdd)
			.setColorWriteMask(vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB)
			.setSrcColorBlendFactor(vk::BlendFactor::eOne)
			.setDstColorBlendFactor(vk::BlendFactor::eOne);

		static vk::PipelineColorBlendAttachmentState MixState
			= vk::PipelineColorBlendAttachmentState()
			.setBlendEnable(true)
			.setAlphaBlendOp(vk::BlendOp::eAdd)
			.setColorBlendOp(vk::BlendOp::eAdd)
			.setColorWriteMask(vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB)
			.setSrcColorBlendFactor(vk::BlendFactor::eOne)
			.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);

		static vk::PipelineColorBlendAttachmentState AlphaBlendState
			= vk::PipelineColorBlendAttachmentState()
			.setBlendEnable(true)
			.setAlphaBlendOp(vk::BlendOp::eAdd)
			.setColorBlendOp(vk::BlendOp::eAdd)
			.setColorWriteMask(vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB)
			.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
			.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);

		switch (blendSetting)
		{
		case BlendSetting::Opaque:
			return OpaqueState;
		case BlendSetting::Add:
			return AddState;
		case BlendSetting::Mixed:
			return MixState;
		case BlendSetting::AlphaBlend:
			return AlphaBlendState;
		}
		return OpaqueState;
	}

	inline vk::SamplerAddressMode ConvertAddressMode(SamplerState::AddressMode addressMode)
	{
		switch (addressMode)
		{
		case SamplerState::AddressMode::Repeat:
			return vk::SamplerAddressMode::eRepeat;
		case SamplerState::AddressMode::Clamp2Edge:
			return vk::SamplerAddressMode::eClampToEdge;
		case SamplerState::AddressMode::Clamp2Border:
			return vk::SamplerAddressMode::eClampToBorder;
		}
		return vk::SamplerAddressMode::eRepeat;
	}

	inline vk::Filter ConvertFilter(SamplerState::FilterType filterType)
	{
		switch (filterType)
		{
		case SamplerState::FilterType::Nearest:
			return vk::Filter::eNearest;
		case SamplerState::FilterType::Linear:
			return vk::Filter::eLinear;
		}
		return vk::Filter::eNearest;
	}

	inline vk::SamplerMipmapMode ConvertMipmapMode(SamplerState::FilterType filterType)
	{
		switch (filterType)
		{
		case SamplerState::FilterType::Nearest:
			return vk::SamplerMipmapMode::eNearest;
		case SamplerState::FilterType::Linear:
			return vk::SamplerMipmapMode::eLinear;
		}
		return vk::SamplerMipmapMode::eNearest;
	}

	inline vk::BorderColor ConvertBorderColor(SamplerState::BorderColor borderColor)
	{
		switch (borderColor)
		{
		case SamplerState::BorderColor::Black:
			return vk::BorderColor::eFloatOpaqueBlack;
		case SamplerState::BorderColor::White:
			return vk::BorderColor::eFloatOpaqueWhite;
		}
		return vk::BorderColor::eFloatOpaqueBlack;
	}

	inline vk::CompareOp ConvertCompOp(CompOp compOp)
	{
		switch (compOp)
		{
		case CompOp::Never:
			return vk::CompareOp::eNever;
		case CompOp::Less:
			return vk::CompareOp::eLess;
		case CompOp::Equal:
			return vk::CompareOp::eEqual;
		case CompOp::LessEqual:
			return vk::CompareOp::eLessOrEqual;
		case CompOp::Greater:
			return vk::CompareOp::eGreater;
		case CompOp::NotEqual:
			return vk::CompareOp::eNotEqual;
		case CompOp::GreaterEqual:
			return vk::CompareOp::eGreaterOrEqual;
		case CompOp::Always:
			return vk::CompareOp::eAlways;
		}
		return vk::CompareOp::eNever;
	}

	inline vk::Format ConvertFormat(Format format)
	{
		switch (format)
		{
		case Format::R8_UINT:
			return vk::Format::eR8Uint;
		case Format::R32_UINT:
			return vk::Format::eR32Uint;
		case Format::R32_FLOAT:
			return vk::Format::eR32Sfloat;
		case Format::RGB8_UNORM:
			return vk::Format::eR8G8B8Unorm;
		case Format::RGBA8_UNORM:
			return vk::Format::eR8G8B8A8Unorm;
		case Format::BGRA8_SRGB:
			return vk::Format::eB8G8R8A8Srgb;
		case Format::RGBA32_FLOAT:
			return vk::Format::eR32G32B32A32Sfloat;
		case Format::D32_FLOAT:
			return vk::Format::eD32Sfloat;
		case Format::D24_UNORM_S8_UINT:
			return vk::Format::eD24UnormS8Uint;
		}
		return vk::Format::eUndefined;
	}

	inline vk::AttachmentLoadOp ConvertLoadOp(AttachmentDesc::LoadOp loadOp)
	{
		switch (loadOp)
		{
		case AttachmentDesc::LoadOp::Load:
			return vk::AttachmentLoadOp::eLoad;
		case AttachmentDesc::LoadOp::Clear:
			return vk::AttachmentLoadOp::eClear;
		case AttachmentDesc::LoadOp::DontCare:
			return vk::AttachmentLoadOp::eDontCare;
		}
		return vk::AttachmentLoadOp::eNoneEXT;
	}

	inline vk::ImageType ConvertImageType(ImageDesc::ImageType imageType)
	{
		switch (imageType)
		{
		case ImageDesc::ImageType::e2D:
			return vk::ImageType::e2D;
		case ImageDesc::ImageType::e3D:
			return vk::ImageType::e3D;
		}
		return vk::ImageType::e2D;
	}
}
