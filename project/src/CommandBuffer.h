#pragma once

#include <vulkan/vulkan_core.h>
#include "LogicalDevice.h"

namespace dae
{
	struct LogicalDevice;

	struct CommandBuffer final
	{
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

		void Begin(VkCommandBufferUsageFlagBits flags = {})
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = flags;

			vkBeginCommandBuffer(commandBuffer, &beginInfo);
		}

		void End()
		{
			vkEndCommandBuffer(commandBuffer);
		}

	};
}