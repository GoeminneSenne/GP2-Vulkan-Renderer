#include "LogicalDevice.h"

#include <set>

#include "Common.h"
#include "DeviceFeatures.h"
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

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pNext = &m_features.features2;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

	if (m_useValidation)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
		createInfo.ppEnabledLayerNames = m_validationLayers.data();
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

dae::LogicalDeviceBuilder& dae::LogicalDeviceBuilder::SetDeviceFeatures(const DeviceFeatures& deviceFeatures)
{
	m_features = deviceFeatures;
	return *this;
}

dae::LogicalDeviceBuilder& dae::LogicalDeviceBuilder::AddDeviceExtension(const char* extensionName)
{
	m_deviceExtensions.push_back(extensionName);
	return *this;
}

dae::LogicalDeviceBuilder& dae::LogicalDeviceBuilder::AddDeviceExtensions(const std::vector<const char*>& extensionNames)
{
	m_deviceExtensions.insert(m_deviceExtensions.end(), extensionNames.begin(), extensionNames.end());
	return *this;
}

dae::LogicalDeviceBuilder& dae::LogicalDeviceBuilder::AddValidationLayer(const char* layerName)
{
	m_validationLayers.push_back(layerName);
	return *this;
}

dae::LogicalDeviceBuilder& dae::LogicalDeviceBuilder::AddValidationLayers(const std::vector<const char*>& layerNames)
{
	m_validationLayers.insert(m_validationLayers.end(), layerNames.begin(), layerNames.end());
	return *this;
}

dae::LogicalDeviceBuilder& dae::LogicalDeviceBuilder::UseValidation(bool use)
{
	m_useValidation = true;
	return *this;
}

dae::Queue dae::LogicalDeviceBuilder::InitQueue(const LogicalDevice& logicalDevice, uint32_t queueFamily) const
{
	VkQueue rawQueue;
	vkGetDeviceQueue(logicalDevice.handle, queueFamily, 0, &rawQueue);

	return { rawQueue, queueFamily };
}

