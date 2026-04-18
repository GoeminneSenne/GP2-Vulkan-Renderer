#pragma once

#include "VulkanLoader.h"

namespace dae
{
	struct Instance
	{
		VkInstance instance;
		VulkanInstanceDebugUtilsFuncTable debugFuncTable;
		VkDebugUtilsMessengerEXT debugMessenger;
	};
}