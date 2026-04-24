#pragma once

#include <vulkan/vulkan_core.h>
#include "Context.h"

namespace dae
{
	struct GraphicsPipeline final
	{
		VkPipeline pipeline = VK_NULL_HANDLE;

		void Destroy(const Context& context) const
		{
			if (pipeline != VK_NULL_HANDLE)
			{
				vkDestroyPipeline(context.logicalDevice.device, pipeline, nullptr);
			}
		}
	};
}