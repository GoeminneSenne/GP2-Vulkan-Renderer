#include "Swapchain.h"

#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "SwapchainSupportDetails.h"

void dae::Swapchain::Create(const PhysicalDevice& physicalDevice, const LogicalDevice& logicalDevice)
{
	const SwapchainSupportDetails& swapChainSupport = physicalDevice.swapchainSupport;

}

void dae::Swapchain::Cleanup(const LogicalDevice& logicalDevice)
{
	for (auto imageView : imageViews)
		vkDestroyImageView(logicalDevice.device, imageView, nullptr);

	vkDestroySwapchainKHR(logicalDevice.device, swapchain, nullptr);
}
