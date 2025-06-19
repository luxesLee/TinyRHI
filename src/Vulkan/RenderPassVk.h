#pragma once
#ifdef RHI_SUPPORT_VULKAN

#include "HeaderVk.h"
#include "UniqueHash.h"

namespace TinyRHI
{
	class RenderPassVk final : public IRenderPass, public UniqueHash
	{
	public:
		RenderPassVk() = delete;
		RenderPassVk(vk::Device logicalDevice, const RenderPassState renderpassState)
		{
			std::vector<vk::AttachmentReference> colorAttachmentRefs;
			std::vector<vk::AttachmentDescription> attachmentDescs;
			for (int i = 0; i < renderpassState.colorAttachs.size(); i++)
			{
				colorAttachmentRefs.push_back(vk::AttachmentReference()
					.setAttachment(i)
					.setLayout(vk::ImageLayout::eColorAttachmentOptimal));

				auto attachmentDesc = vk::AttachmentDescription()
					.setFormat(ConvertFormat(renderpassState.colorAttachs[i].format))
					.setLoadOp(ConvertLoadOp(renderpassState.colorAttachs[i].loadOp))
					.setSamples(vk::SampleCountFlagBits::e1)
					.setStoreOp(vk::AttachmentStoreOp::eStore)
					.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
					.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
					.setInitialLayout(vk::ImageLayout::eUndefined)
					.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

				attachmentDescs.push_back(attachmentDesc);
			}
			needDepth = false;

			auto subpass = vk::SubpassDescription()
				.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
				.setColorAttachmentCount(colorAttachmentRefs.size())
				.setPColorAttachments(colorAttachmentRefs.data());


			if (renderpassState.depthStencilAttach.has_value())
			{
				auto depthAttachmentRef = vk::AttachmentReference()
					.setAttachment(renderpassState.colorAttachs.size())
					.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

				auto attachmentDesc = vk::AttachmentDescription()
					.setFormat(ConvertFormat(renderpassState.depthStencilAttach->format))
					.setLoadOp(ConvertLoadOp(renderpassState.depthStencilAttach->loadOp))
					.setSamples(vk::SampleCountFlagBits::e1)
					.setStoreOp(vk::AttachmentStoreOp::eStore)
					.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
					.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
					.setInitialLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
					.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

				attachmentDescs.push_back(attachmentDesc);

				subpass.setPDepthStencilAttachment(&depthAttachmentRef);
				needDepth = true;
			}

			auto renderPassInfo = vk::RenderPassCreateInfo()
				.setAttachmentCount(attachmentDescs.size())
				.setPAttachments(attachmentDescs.data())
				.setSubpassCount(1)
				.setPSubpasses(&subpass)
				.setDependencyCount(0)
				.setPDependencies(nullptr);

			renderPass = logicalDevice.createRenderPassUnique(renderPassInfo);
		}

		vk::RenderPass RenderPassHandle()
		{
			return renderPass.get();
		}

		Bool HasDepth() const
		{
			return needDepth;
		}

	private:
		vk::UniqueRenderPass renderPass;
		Bool needDepth;
	};
}

#endif
