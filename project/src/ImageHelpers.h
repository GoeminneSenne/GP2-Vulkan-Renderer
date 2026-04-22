#pragma once

#include <stdexcept>

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

			if (vkCreateImageView(logicalDevice.device, &viewInfo, nullptr, &image.view) != VK_SUCCESS)
				throw std::runtime_error("Failed to create image view!");
		}
	}
}
