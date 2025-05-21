#pragma once
#include "HeaderVk.h"

namespace TinyRHI
{

	class SamplerVk : public ISampler
	{
	public:
		SamplerVk() = delete;
		SamplerVk(vk::Device logicalDevice, SamplerState samplerState)
		{
			CreateSampler(logicalDevice, samplerState);
		}

		auto& SamplerHandle() const
		{
			return sampler.get();
		}

		void UpdateSampler(vk::Device logicalDevice, SamplerState samplerState)
		{
			CreateSampler(logicalDevice, samplerState);
		}

	private:
		void CreateSampler(vk::Device logicalDevice, SamplerState samplerState)
		{
			auto addressMode = ConvertAddressMode(samplerState.addressMode);
			auto filterType = ConvertFilter(samplerState.filterType);

			auto samplerInfo = vk::SamplerCreateInfo()
				.setAddressModeU(addressMode)
				.setAddressModeV(addressMode)
				.setAddressModeW(addressMode)
				.setAnisotropyEnable(samplerState.anisotropyEnable)
				.setCompareEnable(samplerState.compareEnable)
				.setCompareOp(ConvertCompOp(samplerState.compOp))
				.setMinFilter(filterType)
				.setMagFilter(filterType)
				.setMaxLod(1e7f)
				.setMinLod(0.0f)
				.setMipmapMode(ConvertMipmapMode(samplerState.samplerMipmap))
				.setUnnormalizedCoordinates(false)
				.setBorderColor(ConvertBorderColor(samplerState.borderColor));

			sampler = logicalDevice.createSamplerUnique(samplerInfo);
		}

	private:
		vk::UniqueSampler sampler;
	};
}