#pragma once
#include <vulkan/vulkan_core.h>

#include "QueueFamilyIndices.h"
#include "SwapchainSupportDetails.h"

namespace dae
{
	struct Instance;

	struct PhysicalDevice final
	{
		VkPhysicalDevice handle = VK_NULL_HANDLE;
		QueueFamilyIndices queueFamilyIndices;
		SwapchainSupportDetails swapchainSupport;
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceFeatures features;
	};

	class PhysicalDeviceSelector final
	{
	public:
		PhysicalDeviceSelector& AddDeviceExtension(const char* extensionName);
		PhysicalDeviceSelector& AddDeviceExtensions(const std::vector<const char*>& extensionNames);

		void PickPhysicalDevice(const Instance& instance, PhysicalDevice& physicalDevice) const;

	private:
		bool IsDeviceSuitable(const Instance& instance, VkPhysicalDevice device) const;
		bool CheckDeviceExtensionsSupport(VkPhysicalDevice device) const;

		std::vector<const char*> deviceExtensions;
	};
}
