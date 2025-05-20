#pragma once
#include "BaseType.h"

namespace TinyRHI
{
	struct BufferType
	{
		Bool bVertex = false;
		Bool bIndex = false;
		Bool bStorage = false;
		Bool bUniform = false;
		Bool bIndirect = false;
		Bool bTransfer = false;
	};

	struct BufferDesc
	{
		BufferType bufferType;
		Uint32 elementNum = 0;
		Uint32 stride = 0;
		Bool bStaging = false;
	};

	class IBuffer
	{
	public:
		virtual void* Map() = 0;
		virtual void UnMap() = 0;
		virtual Uint32 GetSize() const = 0;
	};
}