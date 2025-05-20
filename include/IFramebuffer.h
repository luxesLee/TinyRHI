#pragma once
#include <vector>
#include "BaseType.h"

namespace TinyRHI
{
	class IImageView;
	class IRenderPass;

	struct FramebufferDesc
	{
		Extent2D framebufferExt;
		std::vector<IImageView*> imageViews;
		IRenderPass* renderPass;
	};

	class IFramebuffer
	{
	public:
		virtual ~IFramebuffer() {}
	};
}