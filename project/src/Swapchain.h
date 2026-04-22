#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

#include "Image.h"

namespace dae
{
	struct Instance;
	struct Window;
	struct PhysicalDevice;
	struct LogicalDevice;

	struct Swapchain
	{
		void Create(const Window& window, const Instance& instance, 
			const PhysicalDevice& physicalDevice, const LogicalDevice& logicalDevice);
		void Cleanup(const LogicalDevice& logicalDevice);

		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		std::vector<Image> images;
		VkFormat imageFormat;
		VkExtent2D extent;
	};
}
