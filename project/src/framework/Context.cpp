#include "Context.h"

#include "Common.h"
#include "LogicalDevice.h"
#include "Window.h"
#include "DeviceFeatures.h"
#include "GLFW/glfw3.h"

//VMA includes
#define VMA_IMPLEMENTATION
#define VMA_STATS_STRING_ENABLED 1
#define VMA_DEBUG_LOG_FORMAT
#include <vk_mem_alloc.h>

void dae::Context::Destroy()
{
	if (allocator != nullptr)
	{
		vmaDestroyAllocator(allocator);
	}

	commandPool.Destroy();
	logicalDevice.Destroy();
	instance.Destroy();
}

dae::ContextFactory::ContextFactory(ContextConfig config)
	: m_config{std::move(config)}
{}

void dae::ContextFactory::Create(const Window* pWindow, Context& context)
{
	//Instance
	InstanceBuilder instanceBuilder = {};
	instanceBuilder
		.SetApplicationName("Hello Triangle")
		.SetApiVersion(VK_MAKE_VERSION(1, 0, 0))
		.SetEngineName("No Engine")
		.SetEngineVersion(VK_MAKE_VERSION(1, 0, 0))
		.SetApiVersion(VK_API_VERSION_1_0)
		.UseValidation(m_config.enableValidation)
		.AddValidationLayers(m_config.validationLayers)
		.Build(context.instance);

	//Surface
	LOG_VULKAN_ERROR(
		glfwCreateWindowSurface(context.instance.handle, pWindow->window, nullptr, &context.instance.surface),
		"failed to create window surface!"
	);

	//Required Device Features
	DeviceFeatures features{};
	features.features13.synchronization2 = VK_TRUE;
	features.features2.features.samplerAnisotropy = VK_TRUE;

	//Physical Device
	PhysicalDeviceSelector physicalDeviceSelector = {};
	physicalDeviceSelector
		.AddDeviceExtensions(m_config.deviceExtensions)
		.PickPhysicalDevice(context.instance, context.physicalDevice);

	//Logical Device
	LogicalDeviceBuilder logicalDeviceBuilder = {};
	logicalDeviceBuilder
		.SetDeviceFeatures(features)
		.AddDeviceExtensions(m_config.deviceExtensions)
		.UseValidation(m_config.enableValidation)
		.AddValidationLayers(m_config.validationLayers)
		.Build(context.physicalDevice, context.logicalDevice);

	//Command Pool
	context.commandPool.CreateCommandPool(&context.logicalDevice);

	//VMA Allocator
	VmaAllocatorCreateInfo createInfo{};
	createInfo.physicalDevice = context.physicalDevice.handle;
	createInfo.device = context.logicalDevice.handle;
	createInfo.instance = context.instance.handle;
	createInfo.flags = 0;

	LOG_VULKAN_ERROR(
		vmaCreateAllocator(&createInfo, &context.allocator),
		"failed to create VMA allocator!"
	);
}
