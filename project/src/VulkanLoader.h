#pragma once

#include <vulkan/vulkan_core.h>


namespace dae
{
	
	struct VulkanInstanceDebugUtilsFuncTable
	{
		VkInstance instance = VK_NULL_HANDLE;
		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = nullptr;
		PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;
	};

	inline void LoadInstanceDebugFunctions(VkInstance instance, VulkanInstanceDebugUtilsFuncTable& funcTable)
	{
		funcTable.instance = instance;

		funcTable.vkCreateDebugUtilsMessengerEXT = 
			(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		funcTable.vkDestroyDebugUtilsMessengerEXT =
			(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	}

};



