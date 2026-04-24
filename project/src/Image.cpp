#include "Image.h"
#include "LogicalDevice.h"
#include "Context.h"

void dae::Image::Destroy(const Context& context)
{
	if (image != VK_NULL_HANDLE && allocation != nullptr)
	{
		vmaDestroyImage(context.allocator, image, allocation);
	}

	if (view != VK_NULL_HANDLE)
	{
		vkDestroyImageView(context.logicalDevice.device, view, nullptr);
	}

}
