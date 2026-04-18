#include "InstanceBuilder.h"

#include "GLFW/glfw3.h"
#include "VulkanDebug.h"

void dae::InstanceBuilder::Build(Instance& instance)
{
	if (useValidation && !CheckValidationLayerSupport())
		throw std::runtime_error("Validation layers requested, but not available");

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = applicationName.c_str();
	appInfo.applicationVersion = applicationVersion;
	appInfo.pEngineName = engineName.c_str();
	appInfo.engineVersion = engineVersion;
	appInfo.apiVersion = apiVersion;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto reqExtensions = GetRequiredExtensions();
	extensions.insert(extensions.end(), reqExtensions.begin(), reqExtensions.end());

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (useValidation)
	{
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		PopulateDebugMessengerCreateInfo(debugCreateInfo);

		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
		createInfo.pNext = &debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance.instance) != VK_SUCCESS)
		throw std::runtime_error("failed to create instance!");


	//DEBUG MESSENGER
	/////////////////////
	///TODO: seperate private function?
	if (!useValidation) return;

	LoadInstanceDebugFunctions(instance.instance, instance.debugTable);

	VkDebugUtilsMessengerCreateInfoEXT debugcreateInfo;
	PopulateDebugMessengerCreateInfo(debugcreateInfo);

	if (instance.debugTable.vkCreateDebugUtilsMessengerEXT(instance.instance, &debugcreateInfo, nullptr, &instance.debugMessenger) != VK_SUCCESS)
		throw std::runtime_error("failed to create debug messenger");
}

dae::InstanceBuilder& dae::InstanceBuilder::SetApplicationName(std::string name)
{
	applicationName = std::move(name);
	return *this;
}

dae::InstanceBuilder& dae::InstanceBuilder::SetApplicationVersion(uint32_t version)
{
	applicationVersion = version;
	return *this;
}

dae::InstanceBuilder& dae::InstanceBuilder::SetEngineName(std::string name)
{
	engineName = std::move(name);
	return *this;
}

dae::InstanceBuilder& dae::InstanceBuilder::SetEngineVersion(uint32_t version)
{
	engineVersion = version;
	return *this;
}

dae::InstanceBuilder& dae::InstanceBuilder::SetApiVersion(uint32_t version)
{
	apiVersion = version;
	return *this;
}

dae::InstanceBuilder& dae::InstanceBuilder::AddExtension(const char* extensionName)
{
	extensions.push_back(extensionName);
	return *this;
}

dae::InstanceBuilder& dae::InstanceBuilder::AddValidationLayer(const char* layerName)
{
	validationLayers.push_back(layerName);
	return *this;
}

dae::InstanceBuilder& dae::InstanceBuilder::UseValidation(bool use)
{
	useValidation = use;
	return *this;
}

std::vector<const char*> dae::InstanceBuilder::GetRequiredExtensions() const
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> reqExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (useValidation)
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

	for (const char* layerName : validationLayers)
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
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr; //Optional
}

