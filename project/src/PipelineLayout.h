#pragma once

#include <vulkan/vulkan_core.h>
#include "LogicalDevice.h"

namespace dae
{
	struct PipelineLayout final
	{
		VkPipelineLayout layout = VK_NULL_HANDLE;
		
		void Destroy(const Context& context) const
		{
			if (layout != VK_NULL_HANDLE)
			{
				vkDestroyPipelineLayout(context.logicalDevice.device, layout, nullptr);
			}
		}
	};
}