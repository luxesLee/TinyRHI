#pragma once
#include "HeaderVk.h"
#include <unordered_map>
#include <tuple>

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

	class DescriptorSetLayoutVk
	{
	public:
		DescriptorSetLayoutVk(
			const DeviceData& deviceData,
			std::vector<DescriptorSetLayoutBindingDesc>& _layoutBindings)
		{
			this->layoutBindings = layoutBindings;
			std::vector<vk::DescriptorSetLayoutBinding> dsLayoutBindings(layoutBindings.size());
			for (Uint32 i = 0; i < layoutBindings.size(); i++)
			{
				auto& binding = dsLayoutBindings[i];
				binding.setBinding(layoutBindings[i].binding)
					.setDescriptorCount(1)
					.setDescriptorType(layoutBindings[i].type)
					.setStageFlags(layoutBindings[i].flag);
			}

			auto dsLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo()
				.setBindingCount(dsLayoutBindings.size())
				.setPBindings(dsLayoutBindings.data());
			descriptorSetLayout = deviceData.logicalDevice.createDescriptorSetLayoutUnique(dsLayoutCreateInfo);
		}

		std::vector<DescriptorSetLayoutBindingDesc>& LayoutBinding()
		{
			return layoutBindings;
		}

		Bool Compare(DescriptorSetLayoutVk* other) const
		{
			return other->LayoutBinding() == layoutBindings;
		}

		auto& Handle()
		{
			return descriptorSetLayout.get();
		}

	private:
		vk::UniqueDescriptorSetLayout descriptorSetLayout;
		std::vector<DescriptorSetLayoutBindingDesc> layoutBindings;
	};

	class DescriptorSetVk
	{
	public:
		DescriptorSetVk(
			const DeviceData& _deviceData,
			vk::DescriptorPool descriptorPool,
			DescriptorSetLayoutVk* _layout)
		{
			if (_layout)
			{
				this->descriptorSetLayout = _layout;
				auto descriptorSetAllocInfo = vk::DescriptorSetAllocateInfo()
					.setDescriptorPool(descriptorPool)
					.setDescriptorSetCount(1)
					.setPSetLayouts(&descriptorSetLayout->Handle());
				descriptorSet = _deviceData.logicalDevice.allocateDescriptorSets(descriptorSetAllocInfo)[0];
			}
		}

		Bool Compare(DescriptorSetLayoutVk* other) const
		{
			return descriptorSetLayout->Compare(other);
		}

		auto& Handle()
		{
			return descriptorSet;
		}

	private:
		vk::DescriptorSet descriptorSet;
		DescriptorSetLayoutVk* descriptorSetLayout;
	};

	class DescriptorSetArrayVk
	{
	public:

		VkDescriptorSet* GetIndex(Uint32 index)
		{
			return &dataArray[index];
		}

		vk::DescriptorSet* Handle()
		{

		}

		Uint32 Size() const
		{
			return dataArray.size();
		}

	private:
		std::vector<VkDescriptorSet> dataArray;

	};

	/*
	* layoutBindings <--> descriptorSetLayout <--> descriptorSet
	*/
	class DescriptorSetPoolVk
	{
	public:
		typedef std::vector<DescriptorSetLayoutBindingDesc> DSLayoutKey;
		typedef std::unordered_map<DSLayoutKey, std::unique_ptr<DescriptorSetLayoutVk>> DSLayoutPool;
		typedef std::unordered_map<DescriptorSetLayoutVk*, std::unique_ptr<DescriptorSetVk>> DSPool;

	public:
		DescriptorSetPoolVk(
			const DeviceData& _deviceData);

		DescriptorSetLayoutVk* GetDescriptorSetLayout(DSLayoutKey key)
		{
			auto& vkDSLayout = descriptorSetLayoutCache[key];
			if (!vkDSLayout)
			{
				vkDSLayout = std::make_unique<DescriptorSetLayoutVk>(deviceData, key);
			}
			return vkDSLayout.get();
		}

		DescriptorSetVk* GetDescriptorSet(DescriptorSetLayoutVk* key)
		{
			auto& vkDescriptorSet = descriptorSetCache[key];
			if (!vkDescriptorSet)
			{
				vkDescriptorSet = std::make_unique<DescriptorSetVk>(deviceData, descriptorPool.get(), key);
				return vkDescriptorSet.get();
			}

			if (!vkDescriptorSet->Compare(key))
			{
				vkDescriptorSet = std::make_unique<DescriptorSetVk>(deviceData, descriptorPool.get(), key);
			}

			return vkDescriptorSet.get();
		}

	private:
		const DeviceData& deviceData;
		vk::UniqueDescriptorPool descriptorPool;

		DSLayoutPool descriptorSetLayoutCache;
		DSPool descriptorSetCache;
	};

	// �ռ�д���descriptorset��Ϣ
	class DescriptorSetWriterVk
	{
	public:
		DescriptorSetWriterVk()
		{
			Reset();
		}

		void Reset()
		{
			writeDescriptorSets.clear();
			currentDescriptorSet = nullptr;
		}

		template<Bool bUniform>
		Bool WriteUniformBuffer(vk::Buffer buffer, Uint32 offset, Uint32 range, Uint32 dstBinding);

		template<Bool bStorage>
		Bool WriteImageSampler(vk::ImageView imageView, vk::Sampler sampler, vk::ImageLayout layout, Uint32 dstBinding);

		void BindDescriptor(DescriptorSetVk* vkDescriptorSet)
		{
			for (auto& wdSet : writeDescriptorSets)
			{
				wdSet.setDstSet(vkDescriptorSet->Handle());
			}
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

		auto& GetHandle() const
		{
			return writeDescriptorSets;
		}

	private:
		std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
		DescriptorSetVk* currentDescriptorSet;
		Bool bDirty;
	};
}