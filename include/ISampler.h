#pragma once
#include "BaseType.h"

namespace TinyRHI
{
	struct SamplerState
	{
		enum class AddressMode { Repeat, Clamp2Edge, Clamp2Border, } addressMode = AddressMode::Repeat;
		bool anisotropyEnable = false;
		bool compareEnable = false;
		CompOp compOp = CompOp::Never;
		enum class FilterType { Nearest, Linear, } filterType = FilterType::Nearest;
		enum class BorderColor { Black, White, } borderColor = BorderColor::Black;
		FilterType samplerMipmap = FilterType::Nearest;
	};

	class ISampler
	{
	public:
		virtual ~ISampler() {}
	};
}