#include "Swapchain.h"

#include "Common.h"
#include "Instance.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "SwapchainSupportDetails.h"
#include "ImageHelpers.h"

void dae::Swapchain::Create(const Window& window, const Instance& instance, 
	const PhysicalDevice& physicalDevice, const LogicalDevice& logicalDevice)
{
	const SwapchainSupportDetails& swapchainSupport = physicalDevice.swapchainSupport;

	VkSurfaceFormatKHR surfaceFormat = swapchainSupport.ChooseSurfaceFormat();
	VkPresentModeKHR presentMode = swapchainSupport.ChoosePresentMode();
	VkExtent2D extent = swapchainSupport.ChooseExtent(window);

	uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
	if (swapchainSupport.capabilities.maxImageCount > 0
		&& imageCount > swapchainSupport.capabilities.maxImageCount)
	{
		imageCount = swapchainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = instance.surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	const QueueFamilyIndices& indices = physicalDevice.queueFamilyIndices;
	uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; //Optional
		createInfo.pQueueFamilyIndices = nullptr; //Optional
	}

	createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	LOG_VULKAN_ERROR(
		vkCreateSwapchainKHR(logicalDevice.device, &createInfo, nullptr, &swapchain),
		"failed to create swapchain!"
	);

	vkGetSwapchainImagesKHR(logicalDevice.device, swapchain, &imageCount, nullptr);
	std::vector<VkImage> rawImages(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice.device, swapchain, &imageCount, rawImages.data());


	images.resize(imageCount);
	for (size_t idx{}; idx < imageCount; ++idx)
	{
		images[idx] = ImageHelpers::WrapSwapchainImage(logicalDevice, rawImages[idx], surfaceFormat.format, extent);
	}

	this->imageFormat = surfaceFormat.format;
	this->extent = extent;
}

void dae::Swapchain::Cleanup(const LogicalDevice& logicalDevice, const VmaAllocator& allocator)
{
	for (auto& image : images)
	{
		image.Destroy(logicalDevice, allocator);
	}

	if (swapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(logicalDevice.device, swapchain, nullptr);
	}

}
