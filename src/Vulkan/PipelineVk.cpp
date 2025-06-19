#ifdef RHI_SUPPORT_VULKAN

#include "PipelineVk.h"

using namespace TinyRHI;

GraphicsPipelineVk::GraphicsPipelineVk(
    const DeviceData& deviceData,
    GraphicsPipelineDesc _graphicsPipelineDesc)
    : graphicsPipelineDesc(_graphicsPipelineDesc)
{
    PipelineLayoutVk* vkPipelineLayout = dynamic_cast<PipelineLayoutVk*>(graphicsPipelineDesc.pipelineLayout);
    RenderPassVk* vkRenderPass = dynamic_cast<RenderPassVk*>(graphicsPipelineDesc.renderPass);
    ShaderVk<IShader::Stage::Vertex>* vkVertShader = dynamic_cast<ShaderVk<IShader::Stage::Vertex>*>(graphicsPipelineDesc.vertShader);
    ShaderVk<IShader::Stage::Pixel>* vkPixelShader = dynamic_cast<ShaderVk<IShader::Stage::Pixel>*>(graphicsPipelineDesc.pixelShader);
    this->pipelineLayout = vkPipelineLayout->PipelineLayoutHandle();

    vk::PipelineShaderStageCreateInfo shaderStageCreateInfo[] = { vkVertShader->Handle(), vkPixelShader->Handle() };

    std::vector<vk::VertexInputBindingDescription> vertexBindings;
    std::vector<vk::VertexInputAttributeDescription> attributeDescs;
    for (Int i = 0; i < graphicsPipelineDesc.setting.vertexDecl.vertexBindings.size(); i++)
    {
        auto& binding = graphicsPipelineDesc.setting.vertexDecl.vertexBindings[i];
        vertexBindings.push_back(
            vk::VertexInputBindingDescription()
            .setBinding(binding.binding)	// ÿ���󶨵����һ��VkBuffer
            .setStride(binding.stride)	// ÿ�����������ڻ��������ֽ�
            .setInputRate(binding.bInstance ? vk::VertexInputRate::eInstance : vk::VertexInputRate::eVertex));
    }
    for (Int i = 0; i < graphicsPipelineDesc.setting.vertexDecl.attributeDescs.size(); i++)
    {
        auto& attribDesc = graphicsPipelineDesc.setting.vertexDecl.attributeDescs[i];
        attributeDescs.push_back(
            vk::VertexInputAttributeDescription()
            .setBinding(attribDesc.binding)	// ��VertexInputBindingDescription��Ӧ
            .setLocation(attribDesc.location)	// ��������ɫ����λ��
            .setOffset(attribDesc.offset)	// �Ӷ��㻺������ʼλ�õ���ǰ������ʼ���ֽ�ƫ��
            .setFormat(ConvertAttribType(attribDesc.format)));	// ���Ե����ݸ�ʽ
    }

    auto vertexInputStateCreateInfo = vk::PipelineVertexInputStateCreateInfo()
        .setVertexBindingDescriptionCount(vertexBindings.size())
        .setPVertexBindingDescriptions(vertexBindings.data())
        .setVertexAttributeDescriptionCount(attributeDescs.size())
        .setPVertexAttributeDescriptions(attributeDescs.data());

    auto inputAssemblyCreateInfo = vk::PipelineInputAssemblyStateCreateInfo()
        .setTopology(ConvertPrimitiveTopology(graphicsPipelineDesc.setting.inputAssemlyState.topology))
        .setPrimitiveRestartEnable(graphicsPipelineDesc.setting.inputAssemlyState.bPrimitiveRestart);

    auto rasterizer = vk::PipelineRasterizationStateCreateInfo()
        .setDepthClampEnable(graphicsPipelineDesc.setting.rasterizeState.depthClamp)
        .setPolygonMode(ConvertPolygonMode(graphicsPipelineDesc.setting.rasterizeState.polygonMode))
        .setLineWidth(graphicsPipelineDesc.setting.rasterizeState.lineWidth)
        .setCullMode(ConvertCullMode(graphicsPipelineDesc.setting.rasterizeState.cullMode))
        .setFrontFace(ConvertFrontFace(graphicsPipelineDesc.setting.rasterizeState.frontFace))
        .setDepthBiasEnable(graphicsPipelineDesc.setting.rasterizeState.depthBias);

    auto viewport = vk::PipelineViewportStateCreateInfo()
        .setScissorCount(1)
        .setViewportCount(1);

    auto multisampling = vk::PipelineMultisampleStateCreateInfo()
        .setSampleShadingEnable(graphicsPipelineDesc.setting.samples != MSAASamples::e1)
        .setRasterizationSamples(ConvertMSAASamples(graphicsPipelineDesc.setting.samples));

    std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachmentStates;
    for (const auto& blendSetting : graphicsPipelineDesc.setting.blendSettings)
    {
        colorBlendAttachmentStates.push_back(ConvertBlendState(blendSetting));
    }

    auto colorBlending = vk::PipelineColorBlendStateCreateInfo()
        .setLogicOpEnable(false)
        .setAttachmentCount(colorBlendAttachmentStates.size())
        .setPAttachments(colorBlendAttachmentStates.data());

    auto depthStencil = vk::PipelineDepthStencilStateCreateInfo()
        .setStencilTestEnable(graphicsPipelineDesc.setting.depthState.stencilTest)
        .setDepthTestEnable(graphicsPipelineDesc.setting.depthState.depthTest)
        .setDepthCompareOp(ConvertCompOp(graphicsPipelineDesc.setting.depthState.depthTestComp))
        .setDepthWriteEnable(graphicsPipelineDesc.setting.depthState.depthWrite)
        .setDepthBoundsTestEnable(false);

    vk::DynamicState dynamicStates[] = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
    auto dynamicStateInfo = vk::PipelineDynamicStateCreateInfo()
        .setDynamicStateCount(2)
        .setPDynamicStates(dynamicStates);

    auto pipelineCreateInfo = vk::GraphicsPipelineCreateInfo()
        .setStageCount(2)
        .setPStages(shaderStageCreateInfo)
        .setPVertexInputState(&vertexInputStateCreateInfo)
        .setPInputAssemblyState(&inputAssemblyCreateInfo)
        .setPRasterizationState(&rasterizer)
        .setPViewportState(&viewport)
        .setPMultisampleState(&multisampling)
        .setPDepthStencilState(&depthStencil)
        .setPColorBlendState(&colorBlending)
        .setPDynamicState(&dynamicStateInfo)
        .setLayout(pipelineLayout)
        .setRenderPass(vkRenderPass->RenderPassHandle())
        .setSubpass(0)
        .setBasePipelineHandle(nullptr)
        .setBasePipelineIndex(-1);

    pipeline = deviceData.logicalDevice.createGraphicsPipelineUnique(nullptr, pipelineCreateInfo).value;
}

ComputePipelineVk::ComputePipelineVk(
    const DeviceData& deviceData,
    ComputePipelineDesc _computePipelineDesc)
    : computePipelineDesc(_computePipelineDesc)
{
    PipelineLayoutVk* vkPipelineLayout = dynamic_cast<PipelineLayoutVk*>(computePipelineDesc.pipelineLayout);
    ShaderVk<IShader::Stage::Compute>* vkCompShader = dynamic_cast<ShaderVk<IShader::Stage::Compute>*>(computePipelineDesc.compShader);
    this->pipelineLayout = vkPipelineLayout->PipelineLayoutHandle();

    auto pipelineCreateInfo = vk::ComputePipelineCreateInfo()
        .setFlags(vk::PipelineCreateFlags())
        .setStage(vkCompShader->Handle())
        .setLayout(this->pipelineLayout)
        .setBasePipelineHandle(nullptr)
        .setBasePipelineIndex(-1);

    pipeline = deviceData.logicalDevice.createComputePipelineUnique(nullptr, pipelineCreateInfo).value;
}

#endif
