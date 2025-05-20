#pragma once
#include "BaseType.h"

namespace TinyRHI
{
	struct ShaderDesc
	{
		void* codeData = nullptr;
		Uint32 codeSize = 0;
	};

	class IShader
	{
	public:
		enum class Stage
		{
			Vertex = 0,
			Pixel = 1,
			Geometry = 2,
			Compute,
		};

		virtual ~IShader() {}
	};
}