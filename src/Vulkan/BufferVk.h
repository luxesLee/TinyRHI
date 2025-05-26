#pragma once
#include "HeaderVk.h"
#include "IBuffer.h"
#include "UniqueHash.h"

namespace TinyRHI
{
	class BufferVk : public IBuffer, public UniqueHash
	{
	public:
		BufferVk(
			const DeviceData& _deviceData,
			const BufferDesc& _bufferDesc);

		void SetBufferData(void* data, Uint32 dataSize, Uint32 offset);

		void UpdateBufferData(void* data, Uint32 dataSize, Uint32 offset)
		{
			if (!mappedDataPtr)
			{
				mappedDataPtr = Map();
			}
			memcpy_s(static_cast<std::byte*>(mappedDataPtr) + offset, dataSize, data, dataSize);
		}

		virtual Uint32 GetSize() const
		{
			return size;
		}

		virtual void* Map()
		{
			return deviceData.logicalDevice.mapMemory(bufferMemory.get(), 0, size);
		}

		virtual void UnMap()
		{
			deviceData.logicalDevice.unmapMemory(bufferMemory.get());
		}

		auto& BufferHandle()
		{
			return buffer.get();
		}

	private:
		const DeviceData& deviceData;
		BufferDesc bufferDesc;

		vk::UniqueBuffer buffer;
		vk::UniqueDeviceMemory bufferMemory;

		vk::DeviceSize size;
		void* mappedDataPtr;
	};
}