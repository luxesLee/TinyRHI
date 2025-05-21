#pragma once
#include <memory>
#include "HeaderVk.h"
#include "BufferVk.h"
#include "SamplerVk.h"

namespace TinyRHI
{
	class ImageVk : public IImage
	{
	public:
		ImageVk(
			const DeviceData& _deviceData,
			const ImageDesc& _imageDesc);

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
		const ImageDesc& imageDesc;

		vk::UniqueImage image;
		vk::UniqueDeviceMemory imageMemory;
		vk::DeviceSize size;
	};

	class ImageViewVk : public IImageView
	{
	public:
		ImageViewVk(
			const DeviceData& deviceData,
			const ImageDesc& imageDesc);

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
			const ImageDesc& imageDesc,
			const SamplerState& samplerState)
			: imageView(std::make_unique<ImageViewVk>(deviceData.logicalDevice, imageDesc)),
			sampler(std::make_unique<SamplerVk>(samplerState))
		{
		}

		void SetImageData(void* data, Uint32 dataSize)
		{
			return imageView->SetImageData(data, dataSize);
		}

		void UpdateSampler(vk::Device logicalDevice, SamplerState samplerState)
		{
			sampler->UpdateSampler(logicalDevice, samplerState);
		}

		auto& ImageHandle() const
		{
			return imageView->ImageHandle();
		}

		auto& ImageViewHandle() const
		{
			return imageView->ImageViewHandle();
		}

		auto& SamplerHandle() const
		{
			return sampler->SamplerHandle();
		}

	private:
		std::unique_ptr<ImageViewVk> imageView;
		std::unique_ptr<SamplerVk> sampler;
	};

}