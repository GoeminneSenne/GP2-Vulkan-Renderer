#pragma once

#include <vulkan/vulkan_core.h>

namespace dae
{
	struct LogicalDevice final
	{
		LogicalDevice() = default;
		~LogicalDevice();
		LogicalDevice(const LogicalDevice& other) = delete;
		LogicalDevice(LogicalDevice&& other) = delete;
		LogicalDevice& operator=(const LogicalDevice& other) = delete;
		LogicalDevice& operator=(LogicalDevice&& other) = delete;


		VkDevice device = VK_NULL_HANDLE;
		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkQueue presentQueue = VK_NULL_HANDLE;

		uint32_t graphicsFamily{};
		uint32_t presentFamily{};
	};
}