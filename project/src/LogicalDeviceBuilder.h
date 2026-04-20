#pragma once

#include <vector>

#include "LogicalDevice.h"
#include "QueueFamilyIndices.h"

namespace dae
{
	struct PhysicalDevice;

	class LogicalDeviceBuilder final
	{
	public:
		LogicalDeviceBuilder() = default;

		void Build(const PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice);

		LogicalDeviceBuilder& SetQueueFamilies(QueueFamilyIndices indices);
		LogicalDeviceBuilder& EnablebleAnisotropy(bool enable);
		LogicalDeviceBuilder& AddDeviceExtension(const char* extensionName);
		LogicalDeviceBuilder& AddDeviceExtensions(const std::vector<const char*>& extensionNames);
		LogicalDeviceBuilder& AddValidationLayer(const char* layerName);
		LogicalDeviceBuilder& AddValidationLayers(const std::vector<const char*>& layerNames);
		LogicalDeviceBuilder& UseValidation(bool use);

	private:
		QueueFamilyIndices familyIndices{};
		VkPhysicalDeviceFeatures deviceFeatures{};
		std::vector<const char*> deviceExtensions{};
		std::vector<const char*> validationLayers{};
		bool useValidation{false};

	};
}
