#pragma once

#include "Instance.h"

namespace dae
{
	class InstanceBuilder final
	{
	public:
		explicit InstanceBuilder() = default;

		void Build(VkInstance& instance);

	private:
		//TODO validation layers toevoegen.

		std::string applicationName;
		uint32_t applicationVersion;
		std::string engineName;
		uint32_t engineVersion;
		uint32_t apiVersion;
		std::vector<const char*> extensions;
	};
}