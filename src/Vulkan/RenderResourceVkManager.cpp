#include "RenderResourceVkManager.h"


using namespace TinyRHI;

GraphicsPipelineVk* RenderResourceVkManager::GetGfxPipeline(const GfxSetting &setting, PipelineLayoutVk *pipelineLayout)
{
    Uint hashId = pipelineHash();
    auto& gfxPipeline = gfxPipelineCache[hashId];
    if(!gfxPipeline)
    {
        GraphicsPipelineDesc desc = 
        {
            .vertShader = vertexShader,
            .pixelShader = pixelShader,
            .pipelineLayout = dynamic_cast<IPipelineLayout*>(pipelineLayout),
            .renderPass = GetCurrentRenderPass(),
            .setting = setting,
        };
        gfxPipeline = std::make_unique<GraphicsPipelineVk>(deviceData, desc);
    }
    return gfxPipeline.get();
}

ComputePipelineVk *RenderResourceVkManager::GetComputePipeline(PipelineLayoutVk *pipelineLayout)
{
    Uint hashId = pipelineHash();
    auto& computePipeline = computePipelineCache[hashId];
    if(!computePipeline)
    {
        ComputePipelineDesc desc
        {
            .compShader = compShader,
            .pipelineLayout = pipelineLayout,
        };
        assert(desc.compShader);
        computePipeline = std::make_unique<ComputePipelineVk>(deviceData, desc);
    }
    return computePipeline.get();
}

void RenderResourceVkManager::BeginRenderPass(vk::CommandBuffer cmdBuffer)
{
    FramebufferVk* vkFramebuffer = GetCurrentFramebuffer();
    RenderPassVk* vkRenderPass = GetCurrentRenderPass();

    std::vector<vk::ClearValue> clearValues;
    std::vector<vk::Rect2D> renderAreas;
    for(const auto& colorAttachment : colorAttachments)
    {
        if(colorAttachment)
        {
            clearValues.push_back(colorAttachment->GetClearValue());
            renderAreas.push_back(colorAttachment->GetRenderArea());
        }
    }
    if(depthAttachment)
    {
        clearValues.push_back(depthAttachment->GetClearValue());
        renderAreas.push_back(depthAttachment->GetRenderArea());
    }
    assert(clearValues.size() > 0);
    assert(renderAreas.size() > 0);

    vk::Rect2D renderArea = renderAreas[0];
    for(Uint i = 0; i < renderAreas.size(); i++)
    {
        assert(renderArea == renderAreas[i]);
    }

    vk::RenderPassBeginInfo beginInfo = vk::RenderPassBeginInfo()	
        .setRenderPass(vkRenderPass->RenderPassHandle())
        .setFramebuffer(vkFramebuffer->FramebufferHandle())
        .setRenderArea(renderArea)
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
    Uint32 key = framebufferHash();
    auto& framebuffer = frameBufferCache[key];
    if(!framebuffer)
    {
        FramebufferDesc desc;
        desc.framebufferExt = Extent2D(1024, 1024);
        for(const auto& colorAttachment : colorAttachments)
        {
            if(colorAttachment)
            {
                desc.imageViews.push_back(colorAttachment->ImageViewHandle());
            }
        }
        if(depthAttachment)
        {
            desc.imageViews.push_back(depthAttachment->ImageViewHandle());
        }

        desc.renderPass = GetCurrentRenderPass();

        framebuffer = std::make_unique<FramebufferVk>(deviceData, desc);
    }
    return framebuffer.get();
}

RenderPassVk* RenderResourceVkManager::GetCurrentRenderPass()
{
    RenderPassState state;
    for(const auto& colorAttachment : colorAttachments)
    {
        if(colorAttachment)
        {
            state.colorAttachs.push_back(colorAttachment->AttachmentHandle());
        }
    }
    if(depthAttachment)
    {
        state.depthStencilAttach = depthAttachment->AttachmentHandle();
    }

    Uint32 key = renderPassHash(state);
    auto& renderPass = renderPassCache[key];
    if(!renderPass)
    {
        renderPass = std::make_unique<RenderPassVk>(deviceData.logicalDevice, state);
    }
    return renderPass.get();
}



