#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "Instance.h"

namespace dae
{
	class InstanceBuilder final
	{
	public:
		InstanceBuilder() = default;

		void Build(Instance& instance);

		InstanceBuilder& SetApplicationName(std::string name);
		InstanceBuilder& SetApplicationVersion(uint32_t version);
		InstanceBuilder& SetEngineName(std::string name);
		InstanceBuilder& SetEngineVersion(uint32_t version);
		InstanceBuilder& SetApiVersion(uint32_t version);
		InstanceBuilder& AddExtension(const char* extensionName);
		InstanceBuilder& AddValidationLayer(const char* layerName);
		InstanceBuilder& UseValidation(bool use);

	private:
		std::vector<const char*> GetRequiredExtensions() const;
		bool CheckValidationLayerSupport() const;
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const;

		std::string applicationName{};
		uint32_t applicationVersion{};
		std::string engineName{};
		uint32_t engineVersion{};
		uint32_t apiVersion{};
		std::vector<const char*> extensions{};
		std::vector<const char*> validationLayers{};
		bool useValidation{false};
	};
}