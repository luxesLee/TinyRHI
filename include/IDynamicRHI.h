#pragma once
#include "BaseType.h"
#include "ICmdRHI.h"
#include "IDeviceRHI.h"

namespace TinyRHI
{
	class IDynamicRHI : public ICmdRHI, IDeviceRHI
	{
	public:
		
		virtual IShader* CreateVertexShader(Uint32* data, Uint32 dataSize) = 0;
		virtual IShader* CreatePixelShader(Uint32* data, Uint32 dataSize) = 0;
		virtual IShader* CreateGeometryShader(Uint32* data, Uint32 dataSize) = 0;
		virtual IShader* CreateComputeShader(Uint32* data, Uint32 dataSize) = 0;

	};
}