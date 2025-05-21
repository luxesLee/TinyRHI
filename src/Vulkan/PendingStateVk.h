#pragma
#include "PipelineVk.h"

namespace TinyRHI
{
    class PendingStateVk
    {


    };

    #define MaxVertexCount 20
    class GfxPendingStateVk : public PendingStateVk
    {
    public:
        GfxPendingStateVk()
        {
            Reset();
        }
        ~GfxPendingStateVk();

        void Reset()
        {
            viewport = vk::Viewport();
            bViewportDirty = true;

            scissor = vk::Rect2D();
            bScissorDirty = true;

            currentPipeline = nullptr;
        }

        Bool SetPipeline(GraphicsPipelineVk* newPipeline)
        {
            if(currentPipeline != newPipeline)
            {
                currentPipeline = newPipeline;
                bDescriptorSetDirty = true;
                return true;
            }
            return false;
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
        void SetVertex();
        void SetTexture();
        void SetBuffer();

        void MarkUpdateDynamicStates()
        {
            bViewportDirty = true;
            bScissorDirty = true;
        }

        void PrepareDraw();
        void UpdateDynamicStates()
        {
            vk::CommandBuffer cmdBuffer;

            if(bViewportDirty)
            {
                cmdBuffer.setViewport(0, viewport);
                bViewportDirty = false;
            }

            if(bScissorDirty)
            {
                cmdBuffer.setScissor(0, scissor);
                bScissorDirty = false;
            }
        }

    private:
        vk::Viewport viewport;
        Bool bViewportDirty;

        vk::Rect2D scissor;
        Bool bScissorDirty;

        GraphicsPipelineVk* currentPipeline;

        Bool bDescriptorSetDirty;
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

        void SetTexture();
        void SetBuffer();

        void PrepareDispatch();

    private:
        ComputePipelineVk* currentPipeline;

        Bool bDescriptorSetDirty;

    };

}