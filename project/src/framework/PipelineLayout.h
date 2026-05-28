#pragma once

#include <vulkan/vulkan_core.h>
#include "LogicalDevice.h"

namespace dae
{
	struct PipelineLayout final
	{
		VkPipelineLayout handle = VK_NULL_HANDLE;
		
		void Destroy(const Context& context) const
		{
			if (handle != VK_NULL_HANDLE)
			{
				vkDestroyPipelineLayout(context.logicalDevice.handle, handle, nullptr);
			}
		}
	};
}