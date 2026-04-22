#include "Image.h"
#include "ImageHelpers.h"

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

	ImageHelpers::CreateImageView(logicalDevice, image);

	return image;
}

void dae::Image::Destroy(const LogicalDevice& logicalDevice, const VmaAllocator& allocator)
{
	if (image != VK_NULL_HANDLE && allocation != nullptr)
	{
		vmaDestroyImage(allocator, image, allocation);
	}

	if (view != VK_NULL_HANDLE)
	{
		vkDestroyImageView(logicalDevice.device, view, nullptr);
	}

}
