#include "LogicalDeviceBuilder.h"

#include <set>

#include "Common.h"
#include "PhysicalDevice.h"

void dae::LogicalDeviceBuilder::Build(const PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice)
{
	QueueFamilyIndices familyIndices = physicalDevice.queueFamilyIndices;

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies =
	{
		familyIndices.graphicsFamily.value(),
		familyIndices.presentFamily.value()
	};

	float queuePriority = 1.f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (useValidation)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	LOG_VULKAN_ERROR(
		vkCreateDevice(physicalDevice.physicalDevice, &createInfo, nullptr, &logicalDevice.device),
		"failed to create logical device!"
	);

	logicalDevice.graphicsFamily = familyIndices.graphicsFamily.value();
	logicalDevice.presentFamily = familyIndices.presentFamily.value();

	vkGetDeviceQueue(logicalDevice.device, logicalDevice.graphicsFamily, 0, &logicalDevice.graphicsQueue);
	vkGetDeviceQueue(logicalDevice.device, logicalDevice.presentFamily, 0, &logicalDevice.presentQueue);
}

dae::LogicalDeviceBuilder& dae::LogicalDeviceBuilder::EnablebleAnisotropy(bool enable)
{
	deviceFeatures.samplerAnisotropy = enable ? VK_TRUE : VK_FALSE;
	return *this;
}

dae::LogicalDeviceBuilder& dae::LogicalDeviceBuilder::AddDeviceExtension(const char* extensionName)
{
	deviceExtensions.push_back(extensionName);
	return *this;
}

dae::LogicalDeviceBuilder& dae::LogicalDeviceBuilder::AddDeviceExtensions(const std::vector<const char*>& extensionNames)
{
	deviceExtensions.insert(deviceExtensions.end(), extensionNames.begin(), extensionNames.end());
	return *this;
}

dae::LogicalDeviceBuilder& dae::LogicalDeviceBuilder::AddValidationLayer(const char* layerName)
{
	validationLayers.push_back(layerName);
	return *this;
}

dae::LogicalDeviceBuilder& dae::LogicalDeviceBuilder::AddValidationLayers(const std::vector<const char*>& layerNames)
{
	validationLayers.insert(validationLayers.end(), layerNames.begin(), layerNames.end());
	return *this;
}

dae::LogicalDeviceBuilder& dae::LogicalDeviceBuilder::UseValidation(bool use)
{
	useValidation = true;
	return *this;
}
