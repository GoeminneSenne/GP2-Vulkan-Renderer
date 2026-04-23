#pragma once

#include <vulkan/vulkan_core.h>
 
#include "CommandBuffer.h"

namespace dae
{
	namespace BufferHelpers
	{
		inline void CopyBuffer(const CommandBuffer& commandBuffer, const Buffer& srcBuffer, const Buffer& dstBuffer, VkDeviceSize size)
		{
			VkBufferCopy copyRegion{};
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer.commandBuffer, srcBuffer.buffer, dstBuffer.buffer, 1, &copyRegion);
		}
	}
}