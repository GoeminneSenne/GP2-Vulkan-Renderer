#include  "PhysicalDevice.h"

#include <set>
#include <vector>

#include "Instance.h"
#include "Utils.h"

dae::PhysicalDeviceSelector& dae::PhysicalDeviceSelector::SetRequestedFeatures(const DeviceFeatures& features)
{
	m_requestedFeatures = features;
	return *this;
}

dae::PhysicalDeviceSelector& dae::PhysicalDeviceSelector::AddDeviceExtension(const char* extensionName)
{
	m_deviceExtensions.push_back(extensionName);
	return *this;
}

dae::PhysicalDeviceSelector& dae::PhysicalDeviceSelector::AddDeviceExtensions(
	const std::vector<const char*>& extensionNames)
{
	m_deviceExtensions.insert(m_deviceExtensions.end(), extensionNames.begin(), extensionNames.end());
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

	//Initialize all features structs to be filled in
	VkPhysicalDeviceVulkan13Features availableFeatures13{};
	availableFeatures13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	VkPhysicalDeviceVulkan12Features availableFeatures12{};
	availableFeatures12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	availableFeatures12.pNext = &availableFeatures13;
	VkPhysicalDeviceVulkan11Features availableFeatures11{};
	availableFeatures11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	availableFeatures11.pNext = &availableFeatures12;
	VkPhysicalDeviceFeatures2 availableFeatures{};
	availableFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	availableFeatures.pNext = &availableFeatures11;

	vkGetPhysicalDeviceFeatures2(device, &availableFeatures);
	
	//Check if all required features are available
	bool featuresAvailable = true;
	featuresAvailable &= AreRequiredFeaturesAvailable<VkPhysicalDeviceFeatures>(
		availableFeatures.features, m_requestedFeatures.features2.features);
	featuresAvailable &= AreRequiredFeaturesAvailable<VkPhysicalDeviceVulkan11Features>(
		availableFeatures11, m_requestedFeatures.features11);
	featuresAvailable &= AreRequiredFeaturesAvailable<VkPhysicalDeviceVulkan12Features>(
		availableFeatures12, m_requestedFeatures.features12);
	featuresAvailable &= AreRequiredFeaturesAvailable<VkPhysicalDeviceVulkan13Features>(
		availableFeatures13, m_requestedFeatures.features13);

	return indices.isComplete() && extensionsSupported && swapChainAdequate && featuresAvailable;
}

bool dae::PhysicalDeviceSelector::CheckDeviceExtensionsSupport(VkPhysicalDevice device) const
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}
