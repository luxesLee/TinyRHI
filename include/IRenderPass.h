#pragma once
#include <vector>
#include <optional>
#include "BaseType.h"

namespace TinyRHI
{
	struct AttachmentDesc
	{
		Format format = Format::Undefined;
		enum class LoadOp { Load, Clear, DontCare, } loadOp;
		MSAASamples msaaSamples = MSAASamples::e1;
		ClearValues clearValue;
	};

	struct RenderPassState
	{
		std::vector<AttachmentDesc> colorAttachs;
		std::optional<AttachmentDesc> depthStencilAttach;
	};

	class IRenderPass
	{
	public:
		virtual ~IRenderPass() {}
	};
}