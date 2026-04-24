#pragma once

#include <vulkan/vulkan_core.h>
#include "Queue.h"

namespace dae
{
	struct LogicalDevice final
	{
		void Destroy();

		VkDevice device = VK_NULL_HANDLE;
		Queue graphicsQueue{};
		Queue presentQueue{};
	};
}