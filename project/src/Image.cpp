#include "Image.h"
#include "LogicalDevice.h"

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
