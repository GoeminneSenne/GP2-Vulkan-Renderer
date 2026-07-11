#pragma once

#include <vk_mem_alloc.h>


#include "Buffer.h"
#include "CommandBuffer.h"

namespace dae
{
	namespace BufferHelpers
	{
		inline void CopyBuffer(const CommandBuffer& command, const Buffer& source, const Buffer& dest, VkDeviceSize size)
		{
			VkBufferCopy copyRegion{};
			copyRegion.size = size;
			vkCmdCopyBuffer(command.handle, source.handle, dest.handle, 1, &copyRegion);
		}
	}
}
