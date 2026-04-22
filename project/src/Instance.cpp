#include "Instance.h"
#include "Common.h"
#include "Window.h"

dae::Instance::~Instance()
{
	if (debugMessenger != VK_NULL_HANDLE)
	{
		debugTable.vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
}

void dae::Instance::CreateSurface(const Window& window)
{
	LOG_VULKAN_ERROR(
		glfwCreateWindowSurface(instance, window.window, nullptr, &surface),
		"failed to create window surface!"
	);
}
