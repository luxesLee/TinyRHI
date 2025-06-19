#pragma once
#ifdef RHI_SUPPORT_VULKAN

#include <unordered_map>
#include "IRHIHandle.h"
#include "HeaderVk.h"
#include "FramebufferVk.h"
#include "RenderPassVk.h"
#include "ShaderVk.h"
#include "PipelineVk.h"
#include "BufferVk.h"
#include "ImageViewVk.h"

namespace TinyRHI
{
    
    class RenderResourceVkManager
    {
    public:
        RenderResourceVkManager(const DeviceData& _deviceData)
            : deviceData(_deviceData)
        {
            LoadPipelineCache();
        }

    private:
        void LoadPipelineCache()
        {
        }


    // Pipeline
    public:
        GraphicsPipelineVk* GetGfxPipeline(const GfxSetting& setting, PipelineLayoutVk* pipelineLayout);
        ComputePipelineVk* GetComputePipeline(PipelineLayoutVk* pipelineLayout);

    private:
        std::unordered_map<Uint32, std::unique_ptr<GraphicsPipelineVk>> gfxPipelineCache;
        std::unordered_map<Uint32, std::unique_ptr<ComputePipelineVk>> computePipelineCache;

    public:
        void BeginRenderPass(vk::CommandBuffer cmdBuffer);
        void EndRenderPass(vk::CommandBuffer cmdBuffer);

    // Framebuffer RenderPass
    private:
        FramebufferVk* GetCurrentFramebuffer();
        RenderPassVk* GetCurrentRenderPass();

        std::unordered_map<Uint32, std::unique_ptr<FramebufferVk>> frameBufferCache;
        std::unordered_map<Uint32, std::unique_ptr<RenderPassVk>> renderPassCache;

    public:
        void SetColorAttachments(std::shared_ptr<AttachmentVk> vkAttachment)
        {
            if(attachmentRect == vk::Rect2D())
            {
                attachmentRect = vkAttachment->GetRenderArea();
            }
            else
            {
                assert(attachmentRect == vkAttachment->GetRenderArea());
            }
            colorAttachments.push_back(vkAttachment);
        }
        void SetDepthAttachment(std::shared_ptr<AttachmentVk> vkAttachment)
        {
            if(attachmentRect == vk::Rect2D())
            {
                attachmentRect = vkAttachment->GetRenderArea();
            }
            else
            {
                assert(attachmentRect == vkAttachment->GetRenderArea());
            }
            depthAttachment = vkAttachment;
        }
        void ClearAttachments()
        {
            colorAttachments.clear();
            depthAttachment = nullptr;
            attachmentRect = vk::Rect2D();
        }

    private:
        std::vector<std::shared_ptr<AttachmentVk>> colorAttachments;
        std::shared_ptr<AttachmentVk> depthAttachment;
        vk::Rect2D attachmentRect;



    // Shader
    public:
        template<IShader::Stage stage>
        void SetShader(IShader* shader)
        {
	        ShaderVk<stage>* vkShader = dynamic_cast<ShaderVk<stage>*>(shader);
            assert(vkShader != nullptr);

            if constexpr (stage == IShader::Stage::Vertex)
            {
                vertexShader = vkShader;
            }
            else if constexpr (stage == IShader::Stage::Pixel)
            {
                pixelShader = vkShader;
            }
            else if constexpr (stage == IShader::Stage::Compute)
            {
                compShader = vkShader;
            }
        }

    private:
        ShaderVk<IShader::Stage::Vertex>* vertexShader;
        ShaderVk<IShader::Stage::Pixel>* pixelShader;
        ShaderVk<IShader::Stage::Compute>* compShader;
        

    private:
        const DeviceData& deviceData;
    };

} // namespace TinyRHI

#endif
