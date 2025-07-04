#pragma once
#ifdef RHI_SUPPORT_VULKAN

#include "IRHIHandle.h"
#include "HeaderVk.h"
#include "RenderPassVk.h"
#include "ShaderVk.h"
#include "DescriptorSetPoolVk.h"
#include "UniqueHash.h"

namespace TinyRHI
{
	class PipelineLayoutVk : public IPipelineLayout, public UniqueHash
	{
	public:
		PipelineLayoutVk(
			const DeviceData& deviceData, 
			std::vector<DescriptorSetLayoutVk>& _vkDescriptorSetLayouts)
		{
			std::vector<vk::DescriptorSetLayout> descriptorSetLayouts(_vkDescriptorSetLayouts.size());
			for (Uint32 i = 0; i < _vkDescriptorSetLayouts.size(); i++)
			{
				descriptorSetLayouts[i] = _vkDescriptorSetLayouts[i].DSLayoutHandle();
				vkDescriptorSetLayouts.push_back(std::move(_vkDescriptorSetLayouts[i]));
			}

			auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
				.setSetLayoutCount(descriptorSetLayouts.size())
				.setPSetLayouts(descriptorSetLayouts.data());

			pipelineLayout = deviceData.logicalDevice.createPipelineLayoutUnique(pipelineLayoutCreateInfo);
		}

		auto& PipelineLayoutHandle()
		{
			return pipelineLayout.get();
		}

		auto& DSLayoutHandle()
		{
			return vkDescriptorSetLayouts;
		}

	private:
		vk::UniquePipelineLayout pipelineLayout;
		std::vector<DescriptorSetLayoutVk> vkDescriptorSetLayouts;
	};

	class GraphicsPipelineVk : public IGraphicsPipeline
	{
	public:
		GraphicsPipelineVk(
			const DeviceData& deviceData,
			GraphicsPipelineDesc _graphicsPipelineDesc);

		void Bind(vk::CommandBuffer cmdBuffer)
		{
			cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.get());
		}

		auto& GetLayout() const
		{
			return pipelineLayout;
		}

		auto& PipelineHandle()
		{
			return pipeline.get();
		}

		auto& PipelineDescHandle()
		{
			return graphicsPipelineDesc;
		}

	private:
		vk::UniquePipeline pipeline;
		vk::PipelineLayout pipelineLayout;
		GraphicsPipelineDesc graphicsPipelineDesc;
	};

	class ComputePipelineVk : public IComputePipeline
	{
	public:
		ComputePipelineVk(
			const DeviceData& deviceData,
			ComputePipelineDesc _computePipelineDesc);

		void Bind(vk::CommandBuffer cmdBuffer)
		{
			cmdBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline.get());
		}

		auto& GetLayout() const
		{
			return pipelineLayout;
		}

		auto& PipelineHandle()
		{
			return pipeline.get();
		}

		auto& PipelineDescHandle()
		{
			return computePipelineDesc;
		}

	private:
		vk::UniquePipeline pipeline;
		vk::PipelineLayout pipelineLayout;
		ComputePipelineDesc computePipelineDesc;
	};

}

#endif
