#include "Instance.h"
#include "Common.h"
#include "VulkanDebug.h"
#include "Window.h"

void dae::Instance::Destroy()
{
	if (debugMessenger != VK_NULL_HANDLE)
	{
		debugTable.vkDestroyDebugUtilsMessengerEXT(handle, debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(handle, surface, nullptr);
	vkDestroyInstance(handle, nullptr);
}

void dae::InstanceBuilder::Build(Instance& instance)
{
	if (m_useValidation && !CheckValidationLayerSupport())
		throw std::runtime_error("Validation layers requested, but not available");

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = m_applicationName.c_str();
	appInfo.applicationVersion = m_applicationVersion;
	appInfo.pEngineName = m_engineName.c_str();
	appInfo.engineVersion = m_engineVersion;
	appInfo.apiVersion = m_apiVersion;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto reqExtensions = GetRequiredExtensions();
	m_extensions.insert(m_extensions.end(), reqExtensions.begin(), reqExtensions.end());

	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_extensions.size());
	createInfo.ppEnabledExtensionNames = m_extensions.data();

	if (m_useValidation)
	{
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		PopulateDebugMessengerCreateInfo(debugCreateInfo);

		createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
		createInfo.ppEnabledLayerNames = m_validationLayers.data();
		createInfo.pNext = &debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	LOG_VULKAN_ERROR(
		vkCreateInstance(&createInfo, nullptr, &instance.handle),
		"failed to create instance!"
	);

	//DEBUG MESSENGER
	/////////////////////
	if (!m_useValidation) return;

	LoadInstanceDebugFunctions(instance.handle, instance.debugTable);

	VkDebugUtilsMessengerCreateInfoEXT debugcreateInfo;
	PopulateDebugMessengerCreateInfo(debugcreateInfo);

	LOG_VULKAN_ERROR(
		instance.debugTable.vkCreateDebugUtilsMessengerEXT(instance.handle, &debugcreateInfo, nullptr, &instance.debugMessenger),
		"failed to create debug messenger"
	);
}

dae::InstanceBuilder& dae::InstanceBuilder::SetApplicationName(std::string name)
{
	m_applicationName = std::move(name);
	return *this;
}

dae::InstanceBuilder& dae::InstanceBuilder::SetApplicationVersion(uint32_t version)
{
	m_applicationVersion = version;
	return *this;
}

dae::InstanceBuilder& dae::InstanceBuilder::SetEngineName(std::string name)
{
	m_engineName = std::move(name);
	return *this;
}

dae::InstanceBuilder& dae::InstanceBuilder::SetEngineVersion(uint32_t version)
{
	m_engineVersion = version;
	return *this;
}

dae::InstanceBuilder& dae::InstanceBuilder::SetApiVersion(uint32_t version)
{
	m_apiVersion = version;
	return *this;
}

dae::InstanceBuilder& dae::InstanceBuilder::AddExtension(const char* extensionName)
{
	m_extensions.push_back(extensionName);
	return *this;
}

dae::InstanceBuilder& dae::InstanceBuilder::AddExtensions(const std::vector<const char*>& extensionNames)
{
	m_extensions.insert(m_extensions.end(), extensionNames.begin(), extensionNames.end());
	return *this;
}

dae::InstanceBuilder& dae::InstanceBuilder::AddValidationLayer(const char* layerName)
{
	m_validationLayers.push_back(layerName);
	return *this;
}

dae::InstanceBuilder& dae::InstanceBuilder::AddValidationLayers(const std::vector<const char*>& layerNames)
{
	m_validationLayers.insert(m_validationLayers.end(), layerNames.begin(), layerNames.end());
	return *this;
}

dae::InstanceBuilder& dae::InstanceBuilder::UseValidation(bool use)
{
	m_useValidation = use;
	return *this;
}

std::vector<const char*> dae::InstanceBuilder::GetRequiredExtensions() const
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> reqExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (m_useValidation)
	{
		reqExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return reqExtensions;
}

bool dae::InstanceBuilder::CheckValidationLayerSupport() const
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : m_validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (std::strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
			return false;
	}

	return true;
}

//TODO moet misschien aparte Setters in builder zijn
void dae::InstanceBuilder::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
}

