#pragma once
#include <vulkan/vulkan_core.h>

#include "QueueFamilyIndices.h"
#include "SwapchainSupportDetails.h"

namespace dae
{
	struct PhysicalDevice final
	{
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		QueueFamilyIndices queueFamilyIndices;
		SwapchainSupportDetails swapchainSupport;
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceFeatures features;
	};
}
