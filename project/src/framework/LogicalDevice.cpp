#include "LogicalDevice.h"

#include <set>

#include "Common.h"
#include "PhysicalDevice.h"

void dae::LogicalDevice::Destroy()
{
	vkDestroyDevice(handle, nullptr);
}

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

	//Device Features
	VkPhysicalDeviceVulkan13Features features13{};
	features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	features13.pNext = nullptr;
	features13.synchronization2 = VK_TRUE; //TODO add toggle for feature in builder

	VkPhysicalDeviceVulkan12Features features12{};
	features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	features12.pNext = &features13;

	VkPhysicalDeviceVulkan11Features features11{};
	features11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	features11.pNext = &features12;
	

	VkPhysicalDeviceFeatures2 features2{};
	features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	features2.features = features10;
	features2.pNext = &features11;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pNext = &features2;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	//createInfo.pEnabledFeatures = &deviceFeatures;
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
		vkCreateDevice(physicalDevice.handle, &createInfo, nullptr, &logicalDevice.handle),
		"failed to create logical device!"
	);

	logicalDevice.graphicsQueue = InitQueue(logicalDevice, familyIndices.graphicsFamily.value());
	logicalDevice.presentQueue = InitQueue(logicalDevice, familyIndices.presentFamily.value());
}

dae::LogicalDeviceBuilder& dae::LogicalDeviceBuilder::EnablebleAnisotropy(bool enable)
{
	features10.samplerAnisotropy = enable ? VK_TRUE : VK_FALSE;
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

dae::Queue dae::LogicalDeviceBuilder::InitQueue(const LogicalDevice& logicalDevice, uint32_t queueFamily) const
{
	VkQueue rawQueue;
	vkGetDeviceQueue(logicalDevice.handle, queueFamily, 0, &rawQueue);

	return { rawQueue, queueFamily };
}

