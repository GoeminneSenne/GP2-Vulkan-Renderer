#pragma once
#include "QueueFamilyIndices.h"
#include <vulkan/vulkan_core.h>

namespace dae
{
	struct Context;
}

namespace dae
{
	namespace vkUtils
	{
		inline QueueFamilyIndices FindQueueFamilies(const Instance& instance, VkPhysicalDevice device)
		{
			QueueFamilyIndices indices;

			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

			int i = 0;
			for (const auto& queueFamily : queueFamilies)
			{
				if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					indices.graphicsFamily = i;
				}

				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, instance.surface, &presentSupport);

				if (presentSupport)
				{
					indices.presentFamily = i;
				}

				if (indices.isComplete())
				{
					break;
				}

				++i;
			}

			return indices;
		}

		inline SwapchainSupportDetails QuerySwapChainSupport(const Instance& instance, VkPhysicalDevice device)
		{
			SwapchainSupportDetails details;

			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, instance.surface, &details.capabilities);

			uint32_t formatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, instance.surface, &formatCount, nullptr);

			if (formatCount != 0)
			{
				details.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, instance.surface, &formatCount, details.formats.data());
			}

			uint32_t presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, instance.surface, &presentModeCount, nullptr);

			if (presentModeCount != 0)
			{
				details.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, instance.surface, &presentModeCount, details.presentModes.data());
			}

			return details;
		}
	}
}
