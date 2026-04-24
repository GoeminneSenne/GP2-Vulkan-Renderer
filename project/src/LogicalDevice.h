#pragma once

#include <vulkan/vulkan_core.h>
#include "Queue.h"

namespace dae
{
	struct LogicalDevice final
	{
		void Destroy();

		VkDevice device = VK_NULL_HANDLE;
		Queue graphicsQueue{};
		Queue presentQueue{};
		
		/*
		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkQueue presentQueue = VK_NULL_HANDLE;

		uint32_t graphicsFamily{};
		uint32_t presentFamily{};
		*/
	};
}