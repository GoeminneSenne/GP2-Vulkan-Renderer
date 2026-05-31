#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>
#include "Queue.h"

namespace dae
{
	struct PhysicalDevice;

	struct LogicalDevice final
	{
		void Destroy();

		VkDevice handle = VK_NULL_HANDLE;
		Queue graphicsQueue{};
		Queue presentQueue{};
	};

	class LogicalDeviceBuilder final
	{
	public:
		LogicalDeviceBuilder() = default;

		void Build(const PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice);

		LogicalDeviceBuilder& EnablebleAnisotropy(bool enable);
		LogicalDeviceBuilder& AddDeviceExtension(const char* extensionName);
		LogicalDeviceBuilder& AddDeviceExtensions(const std::vector<const char*>& extensionNames);
		LogicalDeviceBuilder& AddValidationLayer(const char* layerName);
		LogicalDeviceBuilder& AddValidationLayers(const std::vector<const char*>& layerNames);
		LogicalDeviceBuilder& UseValidation(bool use);

	private:
		Queue InitQueue(const LogicalDevice& logicalDevice, uint32_t queueFamily) const;

		VkPhysicalDeviceFeatures features10{};
		std::vector<const char*> deviceExtensions{};
		std::vector<const char*> validationLayers{};
		bool useValidation{ false };

	};
}