#pragma once

#include <vulkan/vulkan_core.h>
#include "LogicalDevice.h"

namespace dae
{
	struct GraphicsPipelineLayout final
	{
		VkPipelineLayout layout = VK_NULL_HANDLE;
		
		void Destroy(const LogicalDevice& logicalDevice) const
		{
			if (layout != VK_NULL_HANDLE)
			{
				vkDestroyPipelineLayout(logicalDevice.device, layout, nullptr);
			}
		}
	};
}