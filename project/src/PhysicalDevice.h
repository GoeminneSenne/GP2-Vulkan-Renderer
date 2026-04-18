#pragma once
#include <vulkan/vulkan_core.h>

namespace dae
{
	struct PhysicalDevice final
	{
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	};
}
