#pragma once

#include <vulkan/vulkan_core.h>
#include "LogicalDevice.h"

namespace dae
{
	struct Sampler final
	{
		VkSampler sampler = VK_NULL_HANDLE;

		void Destroy(const Context& context)
		{
			vkDestroySampler(context.logicalDevice.device, sampler, nullptr);
		}
	};
}