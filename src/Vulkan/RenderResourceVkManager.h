#pragma once
#include <unordered_map>
#include "HeaderVk.h"
#include "FramebufferVk.h"
#include "RenderPassVk.h"
#include "ShaderVk.h"
#include "PipelineVk.h"

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

    public:
        Uint32 pipelineHash()
        {
            return 0U;
        }

        GraphicsPipelineVk* GetGfxPipeline(const GfxSetting& setting, PipelineLayoutVk* pipelineLayout);
        ComputePipelineVk* GetComputePipeline(PipelineLayoutVk* pipelineLayout);

    private:
        std::unordered_map<Uint32, std::unique_ptr<GraphicsPipelineVk>> gfxPipelineCache;
        std::unordered_map<Uint32, std::unique_ptr<ComputePipelineVk>> computePipelineCache;

    public:
        void BeginRenderPass(vk::CommandBuffer cmdBuffer);
        void EndRenderPass(vk::CommandBuffer cmdBuffer);

    private:
        FramebufferVk* GetCurrentFramebuffer();
        RenderPassVk* GetCurrentRenderPass();

        Uint32 framebufferHash()
        {
            return 0U;
        }
        Uint32 renderPassHash(const RenderPassState& state)
        {
            return 0U;
        }

        std::unordered_map<Uint32, std::unique_ptr<FramebufferVk>> frameBufferCache;
        std::unordered_map<Uint32, std::unique_ptr<RenderPassVk>> renderPassCache;

    public:
        void SetColorAttachments(std::shared_ptr<AttachmentVk> vkAttachment)
        {
            colorAttachments.push_back(vkAttachment);
        }
        void SetDepthAttachment(std::shared_ptr<AttachmentVk> vkAttachment)
        {
            depthAttachment = vkAttachment;
        }

    private:
        std::vector<std::shared_ptr<AttachmentVk>> colorAttachments;
        std::shared_ptr<AttachmentVk> depthAttachment;

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
