#pragma once
#include "BaseType.h"

namespace TinyRHI
{
	struct ShaderDesc
	{
		Uint32* codeData = nullptr;
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
			Compute = 0,
		};

		virtual ~IShader() {}
	};
}