#pragma once

#include <vulkan/vulkan_core.h>
#include "LogicalDevice.h"

namespace dae
{
	struct Binding final
	{
		uint32_t binding{};
		VkDescriptorType type{};
		VkShaderStageFlags stages{};
		uint32_t count{};
	};

	struct DescriptorSetLayout final
	{
		VkDescriptorSetLayout layout = VK_NULL_HANDLE;
		std::vector<Binding> bindings;

		void Destroy(const LogicalDevice& logicalDevice) const
		{
			vkDestroyDescriptorSetLayout(logicalDevice.device, layout, nullptr);
		}
	};
}