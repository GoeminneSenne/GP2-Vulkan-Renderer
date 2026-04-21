#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace dae
{
	struct PhysicalDevice;
	struct LogicalDevice;

	struct Swapchain
	{
		void Create(const PhysicalDevice& physicalDevice, const LogicalDevice& logicalDevice);
		void Cleanup(const LogicalDevice& logicalDevice);

		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		VkFormat imageFormat;
		VkExtent2D extent;
		std::vector<VkImage> images;
		std::vector<VkImageView> imageViews;
	
	private:
		void CreateSwapchain();
		void CreateImageViews();	
	};
}