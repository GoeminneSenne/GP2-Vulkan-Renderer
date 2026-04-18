#include "Instance.h"

#include <stdexcept>

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
	if (glfwCreateWindowSurface(instance, window.window, nullptr, &surface) != VK_SUCCESS)
		throw std::runtime_error("failed to create window surface!");
}
