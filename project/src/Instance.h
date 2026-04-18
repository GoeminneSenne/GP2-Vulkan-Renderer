#pragma once

#include "VulkanLoader.h"
#include "vulkan/vulkan.h"


namespace dae
{
	struct Window;

	struct Instance
	{
		Instance() = default;
		~Instance();
		Instance(const Instance& other) = delete;
		Instance(Instance&& other) = delete;
		Instance& operator=(const Instance& other) = delete;
		Instance& operator=(Instance&& other) = delete;

		void CreateSurface(const Window& window);

		VkInstance instance = VK_NULL_HANDLE;
		VulkanInstanceDebugUtilsFuncTable debugTable; //TODO static maken in common file
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
	};
}