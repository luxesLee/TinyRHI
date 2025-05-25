#pragma once
#include "HeaderVk.h"
#include "RenderPassVk.h"
#include "ShaderVk.h"
#include "DescriptorSetPoolVk.h"

namespace TinyRHI
{
	class PipelineLayoutVk : public IPipelineLayout
	{
	public:
		PipelineLayoutVk(
			const DeviceData& deviceData, 
			std::vector<DescriptorSetLayoutVk>& _vkDescriptorSetLayouts)
		{
			std::vector<vk::DescriptorSetLayout> descriptorSetLayouts(_vkDescriptorSetLayouts.size());
			for (Uint32 i = 0; i < _vkDescriptorSetLayouts.size(); i++)
			{
				vkDescriptorSetLayouts.push_back(std::move(_vkDescriptorSetLayouts[i]));
				descriptorSetLayouts[i] = _vkDescriptorSetLayouts[i].DSLayoutHandle();
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
			const GraphicsPipelineDesc& _graphicsPipelineDesc);

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
		const GraphicsPipelineDesc& graphicsPipelineDesc;
	};

	class ComputePipelineVk : public IComputePipeline
	{
	public:
		ComputePipelineVk(
			const DeviceData& deviceData,
			const ComputePipelineDesc& _computePipelineDesc);

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

	private:
		vk::UniquePipeline pipeline;
		vk::PipelineLayout pipelineLayout;
		const ComputePipelineDesc& computePipelineDesc;
	};

}