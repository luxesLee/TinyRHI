#ifdef RHI_SUPPORT_VULKAN

#include "RenderResourceVkManager.h"

using namespace TinyRHI;

GraphicsPipelineVk* RenderResourceVkManager::GetGfxPipeline(const GfxSetting &setting, PipelineLayoutVk *pipelineLayout)
{
    Uint hashResult = 0;
    Uint32 hashMoveIndex = 0;
    auto vkRenderPass = GetCurrentRenderPass();

    hashResult ^= std::hash<Uint32>{}(vertexShader->Hash()) << (hashMoveIndex++);
    hashResult ^= std::hash<Uint32>{}(pixelShader->Hash()) << (hashMoveIndex++);
    hashResult ^= std::hash<Uint32>{}(vkRenderPass->Hash()) << (hashMoveIndex++);
    hashResult ^= std::hash<Uint32>{}(pipelineLayout->Hash()) << (hashMoveIndex++);
    hashResult ^= std::hash<GfxSetting>{}(setting);

    auto& gfxPipeline = gfxPipelineCache[hashResult];
    if(!gfxPipeline)
    {
        GraphicsPipelineDesc desc = 
        {
            .vertShader = vertexShader,
            .pixelShader = pixelShader,
            .pipelineLayout = pipelineLayout,
            .renderPass = vkRenderPass,
            .setting = setting,
        };
        gfxPipeline = std::make_unique<GraphicsPipelineVk>(deviceData, desc);
    }
    return gfxPipeline.get();
}

ComputePipelineVk *RenderResourceVkManager::GetComputePipeline(PipelineLayoutVk *pipelineLayout)
{
    Uint hashResult = 0;
    Uint32 hashMoveIndex = 0;
    hashResult ^= std::hash<Uint32>{}(compShader->Hash()) << (hashMoveIndex++);
    hashResult ^= std::hash<Uint32>{}(pipelineLayout->Hash()) << (hashMoveIndex++);

    auto& computePipeline = computePipelineCache[hashResult];
    if(!computePipeline)
    {
        ComputePipelineDesc desc
        {
            .compShader = compShader,
            .pipelineLayout = pipelineLayout,
        };
        computePipeline = std::make_unique<ComputePipelineVk>(deviceData, desc);
    }
    return computePipeline.get();
}

void RenderResourceVkManager::BeginRenderPass(vk::CommandBuffer cmdBuffer)
{
    assert(depthAttachment || colorAttachments.size() > 0);

    FramebufferVk* vkFramebuffer = GetCurrentFramebuffer();
    auto vkRenderPass = GetCurrentRenderPass();

    std::vector<vk::ClearValue> clearValues;
    for(const auto& colorAttachment : colorAttachments)
    {
        if(colorAttachment)
        {
            clearValues.push_back(colorAttachment->GetClearValue());
        }
    }
    if(depthAttachment)
    {
        clearValues.push_back(depthAttachment->GetClearValue());
    }

    vk::RenderPassBeginInfo beginInfo = vk::RenderPassBeginInfo()	
        .setRenderPass(vkRenderPass->RenderPassHandle())
        .setFramebuffer(vkFramebuffer->FramebufferHandle())
        .setRenderArea(attachmentRect)
        .setClearValueCount(clearValues.size())
        .setPClearValues(clearValues.data());

    cmdBuffer.beginRenderPass(beginInfo, vk::SubpassContents::eInline);  
}

void RenderResourceVkManager::EndRenderPass(vk::CommandBuffer cmdBuffer)
{
    cmdBuffer.endRenderPass();
}

FramebufferVk* RenderResourceVkManager::GetCurrentFramebuffer()
{
    FramebufferDesc desc;
    Uint32 hashResult = 0;
    Uint32 hashMoveIndex = 0;
    auto vkRenderPass = GetCurrentRenderPass();

    for(const auto& colorAttachment : colorAttachments)
    {
        if(colorAttachment)
        {
            desc.imageViews.push_back(colorAttachment->ImageViewHandle());
            hashResult ^= std::hash<Uint32>{}(colorAttachment->ImageViewHandle()->Hash()) << (hashMoveIndex++);
        } 
    }
    if(depthAttachment)
    {
        desc.imageViews.push_back(depthAttachment->ImageViewHandle());
        hashResult ^= std::hash<Uint32>{}(depthAttachment->ImageViewHandle()->Hash()) << (hashMoveIndex++);
    }

    desc.framebufferExt = {attachmentRect.extent.width, attachmentRect.extent.height};
    desc.renderPass = vkRenderPass;

    hashResult ^= std::hash<Uint32>{}(desc.framebufferExt.width) << (hashMoveIndex++);
    hashResult ^= std::hash<Uint32>{}(desc.framebufferExt.height) << (hashMoveIndex++);
    hashResult ^= std::hash<Uint32>{}(vkRenderPass->Hash()) << (hashMoveIndex++);

    auto& framebuffer = frameBufferCache[hashResult];
    if(!framebuffer)
    {
        framebuffer = std::make_unique<FramebufferVk>(deviceData, desc);
    }
    return framebuffer.get();
}

RenderPassVk* RenderResourceVkManager::GetCurrentRenderPass()
{
    assert(depthAttachment || colorAttachments.size() > 0);

    Uint32 hashResult = 0;
    Uint32 hashMoveIndex = 0;
    RenderPassState state;

    auto hashRenderPass = [&](const AttachmentDesc& desc)
    {
        Uint32 result = 0;

        result ^= std::hash<Uint>{}(static_cast<Uint>(desc.format));
        result ^= std::hash<Uint>{}(static_cast<Uint>(desc.loadOp));
        result ^= std::hash<Uint>{}(static_cast<Uint>(desc.msaaSamples));
        for(Uint i = 0; i < 4; i++)
        {
            result ^= std::hash<Float>{}(desc.clearValue.color[i]);
        }
        result ^= std::hash<Float>{}(desc.clearValue.depth);
        result ^= std::hash<Uint32>{}(desc.clearValue.stencil);

        return result;
    };

    for(const auto& colorAttachment : colorAttachments)
    {
        if(colorAttachment)
        {
            state.colorAttachs.push_back(colorAttachment->AttachmentHandle());
            hashResult ^= hashRenderPass(colorAttachment->AttachmentHandle()) << (hashMoveIndex++);
        }
    }
    if(depthAttachment)
    {
        state.depthStencilAttach = depthAttachment->AttachmentHandle();
        hashResult ^= hashRenderPass(depthAttachment->AttachmentHandle()) << (hashMoveIndex++);
    }

    auto& renderPass = renderPassCache[hashResult];
    if(!renderPass)
    {
        renderPass = std::make_unique<RenderPassVk>(deviceData.logicalDevice, state);
    }
    return renderPass.get();
}

#endif

