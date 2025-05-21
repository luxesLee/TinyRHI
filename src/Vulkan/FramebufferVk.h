#pragma once
#include "HeaderVk.h"
#include "RenderPassVk.h"
#include "ImageViewVk.h"

namespace TinyRHI
{
	class FramebufferVk : public IFramebuffer
	{
	public:
		FramebufferVk(
			const DeviceData& deviceData, 
			const FramebufferDesc& framebufferDesc)
		{
			RenderPassVk* renderPass = dynamic_cast<RenderPassVk*>(framebufferDesc.renderPass);

			std::vector<vk::ImageView> vkImageViews;
			for (auto imageView : framebufferDesc.imageViews)
			{
				ImageViewVk* vkImageView = dynamic_cast<ImageViewVk*>(imageView);
				vkImageViews.push_back(vkImageView->Handle());
			}

			auto framebufferInfo = vk::FramebufferCreateInfo()
				.setAttachmentCount(vkImageViews.size())
				.setPAttachments(vkImageViews.data())
				.setRenderPass(renderPass->Handle())
				.setWidth(framebufferDesc.framebufferExt.width)
				.setHeight(framebufferDesc.framebufferExt.height)
				.setLayers(1);

			framebuffer = deviceData.logicalDevice.createFramebufferUnique(framebufferInfo);
		}

		auto& Handle()
		{
			return framebuffer.get();
		}

	private:
		vk::UniqueFramebuffer framebuffer;
	};
}