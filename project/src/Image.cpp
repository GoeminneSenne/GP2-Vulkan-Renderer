#include "Image.h"

#include <stdexcept>

#include "LogicalDevice.h"

dae::Image dae::Image::WrapSwapchainImage(const LogicalDevice& logicalDevice, VkImage swapchainImage, VkFormat format,
                                          VkExtent2D extent)
{
	Image image;
	image.image = swapchainImage;
	image.format = format;
	image.extent = extent;
	image.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image.mipLevels = 1;

	image.CreateImageView(logicalDevice);

	return image;
}

void dae::Image::CreateImageView(const LogicalDevice& logicalDevice)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectMask;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(logicalDevice.device, &viewInfo, nullptr, &view) != VK_SUCCESS)
		throw std::runtime_error("failed to create image view!");
}
