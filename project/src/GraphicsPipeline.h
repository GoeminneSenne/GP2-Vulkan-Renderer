#pragma once

#include <vulkan/vulkan_core.h>
#include "LogicalDevice.h"

namespace dae
{
	struct GraphicsPipeline final
	{
		VkPipeline pipeline = VK_NULL_HANDLE;

		void Destroy(const LogicalDevice& logicalDevice) const
		{
			if (pipeline != VK_NULL_HANDLE)
			{
				vkDestroyPipeline(logicalDevice.device, pipeline, nullptr);
			}
		}
	};
}