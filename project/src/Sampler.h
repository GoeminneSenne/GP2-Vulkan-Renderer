#pragma once

#include <vulkan/vulkan_core.h>
#include "LogicalDevice.h"

namespace dae
{
	struct Sampler final
	{
		VkSampler sampler = VK_NULL_HANDLE;

		void Destroy(const LogicalDevice& logicalDevice)
		{
			vkDestroySampler(logicalDevice.device, sampler, nullptr);
		}
	};
}