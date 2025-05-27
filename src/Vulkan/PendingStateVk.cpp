#include "PendingStateVk.h"
#include "BufferVk.h"
#include "ImageViewVk.h"

using namespace TinyRHI;

PipelineLayoutVk* PendingStateVk::GetPipelineLayout(const DeviceData& deviceData)
{
    Uint32 hashResult = 0;
    std::vector<DescriptorSetLayoutVk> dsLayouts;
    for(Uint i = 0; i < MaxDescriptorSetCount && writerDirty[i]; i++)
    {
        dsLayouts.push_back(DescriptorSetLayoutVk(deviceData, dsWriter[i].GetDSLayoutBindingArray()));
    }

    auto& pipelineLayout = pipelineLayoutCache[hashResult];
    if(!pipelineLayout)
    {
        pipelineLayout = std::make_unique<PipelineLayoutVk>(deviceData, dsLayouts);
    }
    return pipelineLayout.get();
}

void GfxPendingStateVk::PrepareDraw()
{
    UpdateDynamicStates();

    if(dsChanged)
    {
        dsChanged = false;
        // 1. write change to descriptorSet
        for(Uint dsWriterIndex = 0; dsWriterIndex < MaxDescriptorSetCount; dsWriterIndex++)
        {
            if(writerDirty[dsWriterIndex])
            {
				dsWriter[dsWriterIndex].BindDescriptor(*dsArray[dsWriterIndex]);
				dsWriter[dsWriterIndex].Update(deviceData.logicalDevice);
            }
        }

        // 2. bind descriptorSet
        if(dsNum > 0)
        {
            currentCmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, currentPipeline->GetLayout(), 0, dsNum, dsArray[0], 0, nullptr);
        }
    }

    if(bVertDirty)
    {
        bVertDirty = false;
        const VertexDeclaration& vertexDecl = currentPipeline->PipelineDescHandle().setting.vertexDecl;
        if(vertexDecl.attributeDescs.size() == 0)
        {
            return;
        }

        struct TemporaryIA
		{
			vk::Buffer VertexBuffers[MaxVertexCount];
			vk::DeviceSize VertexOffsets[MaxVertexCount];
			Uint32 NumUsed = 0;

			void Add(VkBuffer InBuffer, VkDeviceSize InSize)
			{
				assert(NumUsed < MaxVertexCount);
				VertexBuffers[NumUsed] = InBuffer;
				VertexOffsets[NumUsed] = InSize;
				++NumUsed;
			}
		} TemporaryIA;

        for(Uint bindingId = 0; bindingId < vertexDecl.vertexBindings.size(); bindingId++)
        {
            Uint arrayId = vertexDecl.vertexBindings[bindingId].binding;
            if(vertBufferArray[arrayId] == vk::Buffer())
            {
                continue;
            }

            TemporaryIA.Add(vertBufferArray[arrayId], vertOffsetArray[arrayId]);
        }

        if(TemporaryIA.NumUsed > 0)
        {
            currentCmdBuffer.bindVertexBuffers(0, TemporaryIA.NumUsed, TemporaryIA.VertexBuffers, TemporaryIA.VertexOffsets);
        }
    }
}

void ComputePendingStateVk::PrepareDispatch()
{
    if(dsChanged)
    {
        dsChanged = false;
        // 1. write change to descriptorSet
        for(Uint dsWriterIndex = 0; dsWriterIndex < MaxDescriptorSetCount; dsWriterIndex++)
        {
            if(writerDirty[dsWriterIndex])
            {
                // dsWriter[dsWriterIndex].BindDescriptor(dsArray[dsWriterIndex]);
                // dsWriter[dsWriterIndex].Update();
            }
        }

        // 2. bind descriptorSet
        if(dsNum > 0)
        {
            currentCmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, currentPipeline->GetLayout(), 0, dsNum, dsArray[0], 0, nullptr);
        }
    }
}
