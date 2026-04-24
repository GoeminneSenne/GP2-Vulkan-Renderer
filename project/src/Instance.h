#pragma once

#include "VulkanLoader.h"
#include "vulkan/vulkan.h"


namespace dae
{
	struct Window;

	struct Instance
	{
		void Destroy();

		VkInstance instance = VK_NULL_HANDLE;
		VulkanInstanceDebugUtilsFuncTable debugTable; //TODO static maken in common file
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
	};
}