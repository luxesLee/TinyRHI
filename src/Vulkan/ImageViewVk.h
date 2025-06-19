#pragma once
#ifdef RHI_SUPPORT_VULKAN

#include <memory>
#include "IRHIHandle.h"
#include "HeaderVk.h"
#include "BufferVk.h"
#include "SamplerVk.h"
#include "UniqueHash.h"

namespace TinyRHI
{
	class ImageVk : public IImage
	{
	public:
		ImageVk(
			const DeviceData& _deviceData,
			ImageDesc _imageDesc);

		// For swapChainImageView
		ImageVk(
			const DeviceData& _deviceData,
			vk::Image _image, 
			ImageDesc _imageDesc);

		void SetImageData(void* data, Uint32 dataSize);

		virtual Uint32 GetSize() const
		{
			return size;
		}

		auto& ImageHandle()
		{
			return image.get();
		}

		auto& DescHandle()
		{
			return imageDesc;
		}

	private:
		const DeviceData& deviceData;
		ImageDesc imageDesc;

		vk::UniqueImage image;
		vk::UniqueDeviceMemory imageMemory;
		vk::DeviceSize size;
	};

	class ImageViewVk : public IImageView, public UniqueHash
	{
	public:
		ImageViewVk(
			const DeviceData& deviceData,
			ImageDesc imageDesc);

		// For swapChainImageView
		ImageViewVk(
			const DeviceData& deviceData,
			ImageDesc imageDesc,
			vk::Image image);

		void SetImageData(void* data, Uint32 dataSize)
		{
			imagePtr->SetImageData(data, dataSize);
		}

		virtual Uint32 GetSize() const
		{
			return imagePtr->GetSize();
		}

		auto& ImageViewHandle()
		{
			return imageView.get();
		}

		auto& ImageHandle()
		{
			return imagePtr->ImageHandle();
		}

		auto& DescHandle()
		{
			return imagePtr->DescHandle();
		}

	private:
		vk::UniqueImageView imageView;
		std::unique_ptr<ImageVk> imagePtr;
	};

	class TextureVk : public ITexture
	{
	public:
		TextureVk() = delete;

		TextureVk(			
			const DeviceData& deviceData,
			const ImageDesc& imageDesc)
			: imageView(std::make_unique<ImageViewVk>(deviceData, imageDesc))
		{
		}

		TextureVk(
			const DeviceData& deviceData,
			const ImageDesc& imageDesc,
			const SamplerState& samplerState)
			: imageView(std::make_unique<ImageViewVk>(deviceData, imageDesc)),
			sampler(std::make_unique<SamplerVk>(deviceData.logicalDevice, samplerState))
		{
		}

		void SetImageData(void* data, Uint32 dataSize)
		{
			return imageView->SetImageData(data, dataSize);
		}

		void UpdateSampler(vk::Device logicalDevice, SamplerState samplerState)
		{
			if(!sampler)
			{
				sampler = std::make_unique<SamplerVk>(logicalDevice, samplerState);
			}
			else
			{
				sampler->UpdateSampler(logicalDevice, samplerState);
			}
		}

		auto& ImageHandle() const
		{
			return imageView->ImageHandle();
		}

		auto& ImageViewHandle() const
		{
			return imageView->ImageViewHandle();
		}

		ImageViewVk* ImageViewPtr() const
		{
			return imageView.get();
		}

		auto& SamplerHandle() const
		{
			return sampler->SamplerHandle();
		}

	private:
		std::unique_ptr<ImageViewVk> imageView;
		std::unique_ptr<SamplerVk> sampler;
	};

	// Ptr is managed by outside
	class AttachmentVk
	{
	public:
		AttachmentVk(ImageViewVk* _imageView, AttachmentDesc _attachmentDesc, Bool _bDepth)
			: imageView(_imageView), attachmentDesc(_attachmentDesc), bDepth(_bDepth)
		{
		}

		auto& ImageViewHandle()
		{
			return imageView;
		}

		auto& AttachmentHandle()
		{
			return attachmentDesc;
		}

		vk::ClearValue GetClearValue()
		{
			vk::ClearValue clearValue;
			if(bDepth)
			{
				clearValue.setDepthStencil(vk::ClearDepthStencilValue(
					attachmentDesc.clearValue.depth, 
					attachmentDesc.clearValue.stencil));
			}
			else
			{
				clearValue.setColor(vk::ClearColorValue(
					attachmentDesc.clearValue.color[0], 
					attachmentDesc.clearValue.color[1], 
					attachmentDesc.clearValue.color[2], 
					attachmentDesc.clearValue.color[3]));
			}
			return clearValue;
		}

		vk::Rect2D GetRenderArea()
		{
			vk::Rect2D rect;
			rect.setOffset(vk::Offset2D(0, 0));
			auto size = imageView->DescHandle().size3;
			rect.setExtent(vk::Extent2D(size[0], size[1]));
			return rect;
		}

	private:
		ImageViewVk* imageView;
		AttachmentDesc attachmentDesc;
		Bool bDepth;
	};

}

#endif
