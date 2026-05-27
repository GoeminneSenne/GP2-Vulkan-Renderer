#include "Context.h"

#include "Common.h"
#include "LogicalDevice.h"
#include "Window.h"
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
	CreateInstance(context);
	CreateSurface(context, pWindow);
	PickPhysicalDevice(context);
	CreateLogicalDevice(context);
	CreateCommandPool(context);
	CreateAllocator(context);
}

void dae::ContextFactory::CreateInstance(Context& context)
{
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
}

void dae::ContextFactory::CreateSurface(Context& context, const Window* pWindow)
{
	LOG_VULKAN_ERROR(
		glfwCreateWindowSurface(context.instance.instance, pWindow->window, nullptr, &context.instance.surface),
		"failed to create window surface!"
	);
}

void dae::ContextFactory::PickPhysicalDevice(Context& context)
{
	PhysicalDeviceSelector physicalDeviceSelector = {};
	physicalDeviceSelector
		.AddDeviceExtensions(m_config.deviceExtensions)
		.PickPhysicalDevice(context.instance, context.physicalDevice);
}

void dae::ContextFactory::CreateLogicalDevice(Context& context)
{
	LogicalDeviceBuilder logicalDeviceBuilder = {};
	logicalDeviceBuilder
		.EnablebleAnisotropy(true)
		.AddDeviceExtensions(m_config.deviceExtensions)
		.UseValidation(m_config.enableValidation)
		.AddValidationLayers(m_config.validationLayers)
		.Build(context.physicalDevice,context.logicalDevice);
}

void dae::ContextFactory::CreateCommandPool(Context& context)
{
	context.commandPool.CreateCommandPool(&context.logicalDevice);
}

void dae::ContextFactory::CreateAllocator(Context& context)
{
	VmaAllocatorCreateInfo createInfo{};
	createInfo.physicalDevice = context.physicalDevice.physicalDevice;
	createInfo.device = context.logicalDevice.device;
	createInfo.instance = context.instance.instance;
	createInfo.flags = 0;

	LOG_VULKAN_ERROR(
		vmaCreateAllocator(&createInfo, &context.allocator),
		"failed to create VMA allocator!"
	);
}
