#pragma once
#include <vulkan/vulkan_core.h>

namespace dae
{
	struct DeviceFeatures
	{
		VkPhysicalDeviceFeatures2 features2{};
		VkPhysicalDeviceVulkan11Features features11{};
		VkPhysicalDeviceVulkan12Features features12{};
		VkPhysicalDeviceVulkan13Features features13{};

		DeviceFeatures()
		{
			features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
			features11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
			features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
			features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

			features2.pNext = &features11;
			features11.pNext = &features12;
			features12.pNext = &features13;
		}
	};
}