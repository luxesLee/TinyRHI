#pragma once
#include "HeaderVk.h"
#include "IShader.h"
#include "UniqueHash.h"

namespace TinyRHI
{
	template<IShader::Stage stage>
	class ShaderVk : public IShader, public UniqueHash
	{
	public:
		ShaderVk(
			const DeviceData& deviceData,
			const ShaderDesc& shaderDesc)
		{
			auto shaderModuleCreateInfo = vk::ShaderModuleCreateInfo()
				.setCodeSize(shaderDesc.codeSize)
				.setPCode((Uint32*)shaderDesc.codeData);

			shader = deviceData.logicalDevice.createShaderModuleUnique(shaderModuleCreateInfo);
		}

		vk::PipelineShaderStageCreateInfo Handle()
		{
			auto createInfo = vk::PipelineShaderStageCreateInfo()
				.setModule(shader.get())
				.setPName("main");
			if(stage == IShader::Stage::Vertex)
			{
				createInfo.setStage(vk::ShaderStageFlagBits::eVertex);
			}
			else if(stage == IShader::Stage::Pixel)
			{
				createInfo.setStage(vk::ShaderStageFlagBits::eFragment);
			}
			else if(stage == IShader::Stage::Compute)
			{
				createInfo.setStage(vk::ShaderStageFlagBits::eCompute);
			}
			
			return createInfo;
		}

	private:
		vk::UniqueShaderModule shader;
	};

	
}