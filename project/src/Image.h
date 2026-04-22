#pragma once

#include <vulkan/vulkan_core.h>
#include "vk_mem_alloc.h"


namespace dae
{
	struct LogicalDevice;

	struct Image
	{
		static Image WrapSwapchainImage(const LogicalDevice& logicalDevice,
			VkImage swapchainImage, VkFormat format, VkExtent2D extent);

		VkImage image = VK_NULL_HANDLE;
		VmaAllocation allocation = nullptr;
		VkImageView view = VK_NULL_HANDLE;
		VkFormat format = VK_FORMAT_UNDEFINED;
		VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkExtent2D extent = {};
		VkImageAspectFlags aspectMask = 0;
		uint32_t mipLevels = 0;

	private:
		void CreateImageView(const LogicalDevice& logicalDevice);
		//void GenerateMipmaps();
	};
}
