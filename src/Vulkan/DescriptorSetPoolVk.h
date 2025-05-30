#pragma once
#include "HeaderVk.h"
#include <unordered_map>
#include <tuple>
#include <cassert>

namespace TinyRHI
{
	struct DescriptorSetLayoutBindingDesc
	{
		Uint32 binding;
		vk::DescriptorType type;
		vk::ShaderStageFlags flag;
		Bool operator==(const DescriptorSetLayoutBindingDesc& other) const
		{
			return std::tie(binding, type, flag) == std::tie(other.binding, other.type, other.flag);
		}
	};

	using DescriptorSetLayoutBindingDescArray = std::vector<DescriptorSetLayoutBindingDesc>;

	inline Uint32 ComputeHash(const DescriptorSetLayoutBindingDescArray& layoutBindings)
	{
		Uint32 hashVal = 0;
		const Uint32 prime = 31;
		for(const auto& binding : layoutBindings)
		{
			hashVal = hashVal * prime + std::hash<Uint32>()(binding.binding) 
				+ std::hash<Uint32>()((Uint32)binding.flag) + std::hash<Uint32>()((Uint32)binding.flag);
			hashVal ^= (hashVal >> 16);
		}
		return hashVal;
	}

	class DescriptorSetLayoutVk
	{
	public:
		DescriptorSetLayoutVk() = delete;
		DescriptorSetLayoutVk(
			const DeviceData& deviceData,
			DescriptorSetLayoutBindingDescArray _layoutBindings)
			: layoutBindings(_layoutBindings)
		{
			std::vector<vk::DescriptorSetLayoutBinding> dsLayoutBindings(layoutBindings.size());
			std::vector<vk::DescriptorBindingFlags> bindingFlags(layoutBindings.size());
			for (Uint32 i = 0; i < layoutBindings.size(); i++)
			{
				auto& binding = dsLayoutBindings[i];
				binding.setBinding(layoutBindings[i].binding)
					.setDescriptorCount(1)
					.setDescriptorType(layoutBindings[i].type)
					.setStageFlags(layoutBindings[i].flag);

				bindingFlags[i] = (vk::DescriptorBindingFlagBits::eUpdateAfterBind);
			}

			vk::DescriptorSetLayoutBindingFlagsCreateInfo flag = vk::DescriptorSetLayoutBindingFlagsCreateInfo()
				.setBindingCount(bindingFlags.size()).setBindingFlags(bindingFlags);

			auto dsLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo()
				.setBindingCount(dsLayoutBindings.size())
				.setPBindings(dsLayoutBindings.data())
				.setFlags(vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool)
				.setPNext(&flag);
			descriptorSetLayout = deviceData.logicalDevice.createDescriptorSetLayoutUnique(dsLayoutCreateInfo);

			hashKey = ComputeHash(layoutBindings);
		}

		auto& LayoutBinding()
		{
			return layoutBindings;
		}

		auto& DSLayoutHandle()
		{
			return descriptorSetLayout.get();
		}

		Uint32 Hash() const
		{
			return hashKey;
		}

	private:
		vk::UniqueDescriptorSetLayout descriptorSetLayout;
		DescriptorSetLayoutBindingDescArray layoutBindings;
		Uint32 hashKey;
	};

	class DescriptorSetVk
	{
	public:
		DescriptorSetVk() = delete;
		DescriptorSetVk(
			const DeviceData& _deviceData,
			vk::DescriptorPool _descriptorPool,
			DescriptorSetLayoutVk* _descriptorSetLayout)
			: descriptorSetLayout(_descriptorSetLayout)
		{
			assert(descriptorSetLayout != nullptr);
			auto descriptorSetAllocInfo = vk::DescriptorSetAllocateInfo()
				.setDescriptorPool(_descriptorPool)
				.setDescriptorSetCount(1)
				.setPSetLayouts(&descriptorSetLayout->DSLayoutHandle());
			descriptorSet = _deviceData.logicalDevice.allocateDescriptorSets(descriptorSetAllocInfo)[0];
		}

		auto& DescriptorSetHandle()
		{
			return descriptorSet;
		}

		auto& DesriptorSetLayoutHandle()
		{
			return descriptorSetLayout->DSLayoutHandle();
		}

	private:
		vk::DescriptorSet descriptorSet;
		DescriptorSetLayoutVk* descriptorSetLayout;
	};

	class DescriptorSetWriterVk
	{
	public:
		DescriptorSetWriterVk(Uint descriptorSetNum = 8)
		{
			assert(descriptorSetNum != 0);
			maxDS = descriptorSetNum;
			currentDescriptorSet = nullptr;
			bDirty = false;
			writeDescriptorSets.reserve(20);
			descriptorBufferInfos.reserve(20);
			descriptorImageInfos.reserve(20);
		}

		// offset: 0 default
		// range: bufferSize
		template <Bool bUniform>
		Bool WriteBuffer(vk::Buffer buffer, IShader::Stage stage, Uint32 offset, Uint32 range, Uint32 dstBinding)
		{
			writeDescriptorSets.emplace_back();
			descriptorBufferInfos.emplace_back();
			shaderStages.push_back(ConvertShaderStage(stage));
			assert(writeDescriptorSets.size() < maxDS);

			auto& writeDescriptorSet = writeDescriptorSets.back();
			auto& bufferInfo = descriptorBufferInfos.back();

			bufferInfo.setBuffer(buffer).setOffset(offset).setRange(range);
			writeDescriptorSet.setDstBinding(dstBinding)
				.setDescriptorCount(1)
				.setPBufferInfo(&bufferInfo);
			if (bUniform)
			{
				writeDescriptorSet.setDescriptorType(vk::DescriptorType::eUniformBuffer);
			}
			else
			{
				writeDescriptorSet.setDescriptorType(vk::DescriptorType::eStorageBuffer);
			}

			bDirty = true;
			return true;
		}

		template<Bool bWriteEnable>
		Bool WriteImage(vk::ImageView imageView, IShader::Stage stage, vk::Sampler sampler, Uint32 dstBinding)
		{
			writeDescriptorSets.emplace_back();
			descriptorImageInfos.emplace_back();
			shaderStages.push_back(ConvertShaderStage(stage));
			assert(writeDescriptorSets.size() < maxDS);

			auto& writeDescriptorSet = writeDescriptorSets.back();
			auto& imageInfo = descriptorImageInfos.back();

			imageInfo.setImageView(imageView).setSampler(sampler);
			writeDescriptorSet.setDstBinding(dstBinding).setDescriptorCount(1).setPImageInfo(&imageInfo);
			if (bWriteEnable)
			{
				imageInfo.setImageLayout(vk::ImageLayout::eGeneral);
				writeDescriptorSet.setDescriptorType(vk::DescriptorType::eStorageImage);
			}
			else
			{
				imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
				writeDescriptorSet.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
			}
			
			bDirty = true;
			return true;
		}

		void BindDescriptor(vk::DescriptorSet vkDescriptorSet)
		{
			if(vkDescriptorSet != currentDescriptorSet)
			{
				currentDescriptorSet = vkDescriptorSet;
				for (auto& wdSet : writeDescriptorSets)
				{
					wdSet.setDstSet(currentDescriptorSet);
				}
			}
		}

		DescriptorSetLayoutBindingDescArray GetDSLayoutBindingArray()
		{
			DescriptorSetLayoutBindingDescArray dsLayoutBindingArray;
			for(Uint32 i = 0; i < writeDescriptorSets.size(); i++)
			{
				auto& writeDescriptorSet = writeDescriptorSets[i];
				DescriptorSetLayoutBindingDesc dsLayoutBindingDesc;
				
				dsLayoutBindingDesc.binding = writeDescriptorSet.dstBinding;
				dsLayoutBindingDesc.type = writeDescriptorSet.descriptorType;
				dsLayoutBindingDesc.flag = shaderStages[i];
				dsLayoutBindingArray.push_back(dsLayoutBindingDesc);
			}
			return dsLayoutBindingArray;
		}

		Bool Update(vk::Device logicalDevice)
		{
			if (bDirty)
			{
				logicalDevice.updateDescriptorSets(writeDescriptorSets, {});
				bDirty = false;
				return true;
			}
			return false;
		}

		Bool Dirty() const
		{
			return bDirty;
		}

		void Reset()
		{
			currentDescriptorSet = vk::DescriptorSet();
			writeDescriptorSets.clear();
			shaderStages.clear();
			descriptorBufferInfos.clear();
			descriptorImageInfos.clear();
			bDirty = false;
		}

	private:
		std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
		std::vector<vk::ShaderStageFlags> shaderStages;
		std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos;
		std::vector<vk::DescriptorImageInfo> descriptorImageInfos;
		vk::DescriptorSet currentDescriptorSet;
		Bool bDirty;
		Uint maxDS;
	};

	/*
	* layoutBindings <--> descriptorSetLayout <--> descriptorSet
	*/
	class DescriptorSetPoolVk
	{
	public:
		DescriptorSetPoolVk(
			const DeviceData& _deviceData);
		~DescriptorSetPoolVk() {}

		DescriptorSetLayoutVk* GetDescriptorSetLayout(const DescriptorSetLayoutBindingDescArray& layoutBindings)
		{
			Uint32 hashId = ComputeHash(layoutBindings);
			auto& vkDescriptorSetLayoutVk = descriptorSetLayoutCache[hashId];
			if(!vkDescriptorSetLayoutVk)
			{
				vkDescriptorSetLayoutVk = std::make_unique<DescriptorSetLayoutVk>(deviceData, layoutBindings);
			}
			return vkDescriptorSetLayoutVk.get();
		}

		DescriptorSetVk* GetDescriptorSet(const DescriptorSetLayoutBindingDescArray& layoutBindings)
		{
			Uint32 hashId = ComputeHash(layoutBindings);
			auto& vkDescriptorSet = descriptorSetCache[hashId];
			if(!vkDescriptorSet)
			{
				DescriptorSetLayoutVk* vkDescriptorSetLayout = GetDescriptorSetLayout(layoutBindings);
				vkDescriptorSet = std::make_unique<DescriptorSetVk>(deviceData, descriptorPool.get(), vkDescriptorSetLayout);
			}
			return vkDescriptorSet.get();
		}

		DescriptorSetVk* GetDescriptorSet(DescriptorSetLayoutVk* dsLayout)
		{
			if(!dsLayout)
			{
				return nullptr;
			}

			Uint32 hashId = dsLayout->Hash();
			auto& vkDescriptorSet = descriptorSetCache[hashId];
			if(!vkDescriptorSet)
			{
				if(!descriptorSetLayoutCache[hashId])
				{
					descriptorSetLayoutCache[hashId] = std::unique_ptr<DescriptorSetLayoutVk>(dsLayout);
				}
				vkDescriptorSet = std::make_unique<DescriptorSetVk>(deviceData, descriptorPool.get(), dsLayout);
			}
			return vkDescriptorSet.get();
		}

	private:
		const DeviceData& deviceData;
		vk::UniqueDescriptorPool descriptorPool;
		std::unordered_map<Uint32, std::unique_ptr<DescriptorSetLayoutVk>> descriptorSetLayoutCache;
		std::unordered_map<Uint32, std::unique_ptr<DescriptorSetVk>> descriptorSetCache;
	};

}