#pragma once

#include "VulkanLoader.h"
#include "vulkan/vulkan.h"


namespace dae
{
	struct Instance
	{
		~Instance();

		VkInstance instance{};
		VulkanInstanceDebugUtilsFuncTable debugTable{}; //TODO static maken in common file
		VkDebugUtilsMessengerEXT debugMessenger{};
	};
}