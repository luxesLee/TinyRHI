#pragma once
#include <cassert>
#include "PipelineVk.h"
#include "DescriptorSetPoolVk.h"
#include "ImageViewVk.h"
#include "BufferVk.h"

namespace TinyRHI
{
    #define MaxVertexCount 20
    #define MaxDescriptorSetCount 4

    class PendingStateVk
    {
    public:
        PendingStateVk(const DeviceData& _deviceData)
            : deviceData(_deviceData)
        {
            dsPool = std::make_unique<DescriptorSetPoolVk>(_deviceData);
            for(Uint i = 0; i < MaxDescriptorSetCount; i++)
            {
                writerDirty[i] = false;
            }
        }

        void SetCmdBuffer(vk::CommandBuffer cmdBuffer)
        {
            currentCmdBuffer = cmdBuffer;
        }

        void SetSamplerImage(TextureVk* vkTexture, IShader::Stage stage, Uint setId, Uint bindingId)
        {
            return SetTexture<false>(vkTexture, stage, setId, bindingId);
        }

        void SetStorageImage(TextureVk* vkTexture, IShader::Stage stage, Uint setId, Uint bindingId)
        {
            return SetTexture<true>(vkTexture, stage, setId, bindingId);
        }

        void SetStorageBuffer(BufferVk* vkBuffer, IShader::Stage stage, Uint setId, Uint bindingId)
        {
            return SetBuffer<false>(vkBuffer, stage, setId, bindingId);
        }
        void SetUniformBuffer(BufferVk* vkBuffer, IShader::Stage stage, Uint setId, Uint bindingId)
        {
            return SetBuffer<true>(vkBuffer, stage, setId, bindingId);
        }

        PipelineLayoutVk* GetPipelineLayout(const DeviceData& deviceData);

    protected:
        template<Bool bWriteEnable>
        void SetTexture(TextureVk* vkTexture, IShader::Stage stage, Uint setId, Uint bindingId)
        {
            Dirty(dsWriter[setId].WriteImage<bWriteEnable>(vkTexture->ImageViewHandle(), stage, vkTexture->SamplerHandle(), bindingId), setId);
        }

        template<Bool bUniform>
        void SetBuffer(BufferVk* vkBuffer, IShader::Stage stage, Uint setId, Uint bindingId)
        {
            Dirty(dsWriter[setId].WriteBuffer<bUniform>(vkBuffer->BufferHandle(), stage, 0, vkBuffer->GetSize(), bindingId), setId);
        }

        void Dirty(Bool bDirty, Uint index)
        {
            if(bDirty && index < MaxDescriptorSetCount)
            {
                writerDirty[index] = true;
                dsChanged = true;
            }
        }

        void Reset()
        {
            for (Uint i = 0; i < MaxDescriptorSetCount && writerDirty[i]; i++)
            {
                dsWriter[i].Reset();
                writerDirty[i] = false;
            }
        }

    protected:
        vk::CommandBuffer currentCmdBuffer;

        DescriptorSetWriterVk dsWriter[MaxDescriptorSetCount];
        Bool writerDirty[MaxDescriptorSetCount];
        Bool dsChanged;

        vk::DescriptorSet* dsArray[MaxDescriptorSetCount];
        Uint dsNum;

        const DeviceData& deviceData;
        std::unique_ptr<DescriptorSetPoolVk> dsPool;

        std::unordered_map<Uint32, std::unique_ptr<PipelineLayoutVk>> pipelineLayoutCache;
    };

    class GfxPendingStateVk : public PendingStateVk
    {
    public:
        GfxPendingStateVk(const DeviceData& _deviceData)
            : PendingStateVk(_deviceData)
        {
            Reset();
        }
        ~GfxPendingStateVk()
        {
        }

        void Reset()
        {
            PendingStateVk::Reset();
            viewport = vk::Viewport();
            bViewportDirty = true;

            scissor = vk::Rect2D();
            bScissorDirty = true;

            bVertDirty = true;

            currentPipeline = nullptr;
        }

        Bool SetPipeline(GraphicsPipelineVk* newPipeline)
        {
            if(currentPipeline != newPipeline)
            {
                currentPipeline = newPipeline;
                auto& pipelineDesc = currentPipeline->PipelineDescHandle();
                PipelineLayoutVk* vkPipelineLayout = dynamic_cast<PipelineLayoutVk*>(pipelineDesc.pipelineLayout);
                if(vkPipelineLayout)
                {
                    auto& dsVkArray = vkPipelineLayout->DSLayoutHandle();
                    for(dsNum = 0; dsNum < dsVkArray.size() && dsNum < MaxDescriptorSetCount; dsNum++)
                    {
                        dsArray[dsNum] = &dsPool->GetDescriptorSet(&dsVkArray[dsNum])->DescriptorSetHandle();
                    }
                }
                return true;
            }
            return false;
        }

        void Bind()
        {
            currentPipeline->Bind(currentCmdBuffer);
        }

        void SetViewport(vk::Viewport newViewport)
        {
            if(viewport != newViewport)
            {
                viewport = newViewport;
                bViewportDirty = true;
                // SetScissor();
            }
        }

        void SetScissor(vk::Rect2D newScissor)
        {
            if(scissor != newScissor)
            {
                scissor = newScissor;
                bScissorDirty = true;
            }
        }

        void SetVertex(Uint32 vertId, vk::Buffer vertBuffer, Uint32 offset)
        {
            if(vertId < MaxVertexCount && vertBufferArray[vertId] != vertBuffer && vertOffsetArray[vertId] != offset)
            {
                vertBufferArray[vertId] = vertBuffer;
                vertOffsetArray[vertId] = offset;
                bVertDirty = true;
            }
        }

        void MarkUpdateDynamicStates()
        {
            bViewportDirty = true;
            bScissorDirty = true;
        }

        void PrepareDraw();

        void UpdateDynamicStates()
        {
            if(bViewportDirty)
            {
                currentCmdBuffer.setViewport(0, viewport);
                bViewportDirty = false;
            }

            if(bScissorDirty)
            {
                currentCmdBuffer.setScissor(0, scissor);
                bScissorDirty = false;
            }
        }

    private:
        vk::Viewport viewport;
        Bool bViewportDirty;

        vk::Rect2D scissor;
        Bool bScissorDirty;

        vk::Buffer vertBufferArray[MaxVertexCount];
        vk::DeviceSize vertOffsetArray[MaxVertexCount];
        Bool bVertDirty;

        GraphicsPipelineVk* currentPipeline;
    };

    class ComputePendingStateVk : public PendingStateVk
    {
    public:
        ComputePendingStateVk(const DeviceData& _deviceData)
            : PendingStateVk(_deviceData)
        {
        }
        ~ComputePendingStateVk()
        {
        }

        void Reset()
        {
            PendingStateVk::Reset();
            currentPipeline = nullptr;
        }

        Bool SetPipeline(ComputePipelineVk* newPipeline)
        {
            if(currentPipeline != newPipeline)
            {
                currentPipeline = newPipeline;
                auto& pipelineDesc = currentPipeline->PipelineDescHandle();
                PipelineLayoutVk* vkPipelineLayout = dynamic_cast<PipelineLayoutVk*>(pipelineDesc.pipelineLayout);
                if(vkPipelineLayout)
                {
                    auto& dsVkArray = vkPipelineLayout->DSLayoutHandle();
                    for(dsNum = 0; dsNum < dsVkArray.size() && dsNum < MaxDescriptorSetCount; dsNum++)
                    {
                        dsArray[dsNum] = &dsPool->GetDescriptorSet(&dsVkArray[dsNum])->DescriptorSetHandle();
                    }
                }
                return true;
            }
            return false;
        }

        void Bind()
        {
            currentPipeline->Bind(currentCmdBuffer);
        }

        void PrepareDispatch();

    private:
        ComputePipelineVk* currentPipeline;
    };

}