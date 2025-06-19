#pragma once
#ifdef RHI_SUPPORT_VULKAN

#include "HeaderVk.h"
#include "IRHIHandle.h"
#include "RenderPassVk.h"
#include "ImageViewVk.h"

namespace TinyRHI
{
	class FramebufferVk : public IFramebuffer
	{
	public:
		FramebufferVk(
			const DeviceData& _deviceData, 
			const FramebufferDesc& _framebufferDesc)
			: framebufferDesc(_framebufferDesc)
		{
			renderPass = dynamic_cast<RenderPassVk*>(framebufferDesc.renderPass);

			std::vector<vk::ImageView> vkImageViews;
			for (auto imageView : framebufferDesc.imageViews)
			{
				ImageViewVk* vkImageView = dynamic_cast<ImageViewVk*>(imageView);
				vkImageViews.push_back(vkImageView->ImageViewHandle());
			}

			auto framebufferInfo = vk::FramebufferCreateInfo()
				.setAttachmentCount(vkImageViews.size())
				.setPAttachments(vkImageViews.data())
				.setRenderPass(renderPass->RenderPassHandle())
				.setWidth(framebufferDesc.framebufferExt.width)
				.setHeight(framebufferDesc.framebufferExt.height)
				.setLayers(1);

			framebuffer = _deviceData.logicalDevice.createFramebufferUnique(framebufferInfo);
		}

		auto& FramebufferHandle()
		{
			return framebuffer.get();
		}

		auto& RenderPassHandle()
		{
			return renderPass;
		}

	private:
		vk::UniqueFramebuffer framebuffer;
		const FramebufferDesc& framebufferDesc;
		RenderPassVk* renderPass;
	};
}

#endif
