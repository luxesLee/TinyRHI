#pragma
#include <cassert>
#include "PipelineVk.h"
#include "DescriptorSetPoolVk.h"

namespace TinyRHI
{
    #define MaxVertexCount 20
    #define MaxDescriptorSetCount 4

    class TextureVk;
    class BufferVk;

    class PendingStateVk
    {
    public:
        PendingStateVk()
        {
            for(Uint i = 0; i < MaxDescriptorSetCount; i++)
            {
                // dsWriter;
            }
        }

        void SetCmdBuffer(vk::CommandBuffer cmdBuffer)
        {
            currentCmdBuffer = cmdBuffer;
        }

        void SetSamplerImage(TextureVk* vkTexture, Uint setId, Uint bindingId)
        {
            return SetTexture<false>(vkTexture, setId, bindingId);
        }

        void SetStorageImage(TextureVk* vkTexture, Uint setId, Uint bindingId)
        {
            return SetTexture<true>(vkTexture, setId, bindingId);
        }

        void SetStorageBuffer(BufferVk* vkBuffer, Uint setId, Uint bindingId)
        {
            return SetBuffer<false>(vkBuffer, setId, bindingId);
        }
        void SetUniformBuffer(BufferVk* vkBuffer, Uint setId, Uint bindingId)
        {
            return SetBuffer<true>(vkBuffer, setId, bindingId);
        }

    protected:
        template<bool bWriteEnable>
        void SetTexture(TextureVk* vkTexture, Uint setId, Uint bindingId)
        {
            Dirty(dsWriter[setId].WriteImage<bWriteEnable>(vkTexture->ImageViewHandle(), vkTexture->SamplerHandle(), bindingId), setId);
        }

        template<Bool bUniform>
        void SetBuffer(BufferVk* vkBuffer, Uint setId, Uint bindingId)
        {
            Dirty(dsWriter[setId].WriteBuffer<bUniform>(vkBuffer->BufferHandle(), 0, vkBuffer->GetSize(), bindingId), setId);
        }

        void Dirty(Bool bDirty, Uint index)
        {
            if(bDirty && index < MaxDescriptorSetCount)
            {
                writerDirty[index] = true;
                dsChanged = true;
            }
        }

    protected:
        vk::CommandBuffer currentCmdBuffer;

        DescriptorSetWriterVk dsWriter[MaxDescriptorSetCount];
        Bool writerDirty[MaxDescriptorSetCount];
        Bool dsChanged;

        vk::DescriptorSet* dsArray[MaxDescriptorSetCount];
        Uint dsNum;
        std::unique_ptr<DescriptorSetPoolVk> dsPool;
    };

    class GfxPendingStateVk : public PendingStateVk
    {
    public:
        GfxPendingStateVk()
        {
            Reset();
        }
        ~GfxPendingStateVk()
        {
        }

        void Reset()
        {
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
                PipelineLayoutVk* vkPipelineLayout = dynamic_cast<PipelineLayoutVk*>(currentPipeline->PipelineDescHandle().pipelineLayout);
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
        ComputePendingStateVk();
        ~ComputePendingStateVk();

        Bool SetPipeline(ComputePipelineVk* newPipeline)
        {
            if(currentPipeline != newPipeline)
            {
                currentPipeline = newPipeline;
                return true;
            }
            return false;
        }

        void PrepareDispatch();

    private:
        ComputePipelineVk* currentPipeline;
    };

}