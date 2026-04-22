#pragma once

#include <stdexcept>

#include <vulkan/vulkan_core.h>

#include "Common.h"
#include "Image.h"
#include "LogicalDevice.h"

namespace dae
{
	namespace ImageHelpers
	{
		inline void CreateImageView(const LogicalDevice& logicalDevice, Image& image)
		{
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image.image;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = image.format;
			viewInfo.subresourceRange.aspectMask = image.aspectMask;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = image.mipLevels;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			LOG_VULKAN_ERROR(
				vkCreateImageView(logicalDevice.device, &viewInfo, nullptr, &image.view),
				"failed to create image view!"
			);
		}

		inline Image WrapSwapchainImage(const LogicalDevice& logicalDevice, 
			VkImage swapchainImage, VkFormat format, VkExtent2D extent)
		{
			Image image;
			image.image = swapchainImage;
			image.format = format;
			image.extent = extent;
			image.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			image.mipLevels = 1;

			CreateImageView(logicalDevice, image);

			return image;
		}
	}
}
