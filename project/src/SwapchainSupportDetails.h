#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

namespace dae
{
	struct Window;

	struct SwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;

        VkSurfaceFormatKHR ChooseSurfaceFormat() const;
        VkPresentModeKHR ChoosePresentMode() const;
    	VkExtent2D ChooseExtent(const Window& window) const;
    };
}