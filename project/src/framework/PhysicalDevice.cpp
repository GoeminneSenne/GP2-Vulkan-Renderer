#include  "PhysicalDevice.h"

#include <set>
#include <vector>

#include "Instance.h"
#include "Utils.h"

dae::PhysicalDeviceSelector& dae::PhysicalDeviceSelector::AddDeviceExtension(const char* extensionName)
{
	deviceExtensions.push_back(extensionName);
	return *this;
}

dae::PhysicalDeviceSelector& dae::PhysicalDeviceSelector::AddDeviceExtensions(
	const std::vector<const char*>& extensionNames)
{
	deviceExtensions.insert(deviceExtensions.end(), extensionNames.begin(), extensionNames.end());
	return *this;
}

void dae::PhysicalDeviceSelector::PickPhysicalDevice(const Instance& instance, PhysicalDevice& physicalDevice) const
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance.handle, &deviceCount, nullptr);

	if (deviceCount == 0)
		throw std::runtime_error("failed to find GPUs with Vulkan support!");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance.handle, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (IsDeviceSuitable(instance, device))
		{
			physicalDevice.handle = device;
			physicalDevice.queueFamilyIndices = vkUtils::FindQueueFamilies(instance, device);
			physicalDevice.swapchainSupport = vkUtils::QuerySwapChainSupport(instance, device);
			vkGetPhysicalDeviceProperties(device, &physicalDevice.properties);
			vkGetPhysicalDeviceFeatures(device, &physicalDevice.features);
			break;
		}
	}

	if (physicalDevice.handle == VK_NULL_HANDLE)
		throw std::runtime_error("failed to find a suitable GPU");
}

bool dae::PhysicalDeviceSelector::IsDeviceSuitable(const Instance& instance, VkPhysicalDevice device) const
{
	QueueFamilyIndices indices = vkUtils::FindQueueFamilies(instance, device);

	bool extensionsSupported = CheckDeviceExtensionsSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapchainSupportDetails swapchainSupport = vkUtils::QuerySwapChainSupport(instance, device);
		swapChainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

bool dae::PhysicalDeviceSelector::CheckDeviceExtensionsSupport(VkPhysicalDevice device) const
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}
