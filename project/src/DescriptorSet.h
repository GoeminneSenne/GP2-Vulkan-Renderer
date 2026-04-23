#pragma once

#include <vulkan/vulkan_core.h>

namespace dae
{
	struct DescriptorSetLayout;

	struct DescriptorSet final
	{
		VkDescriptorSet set = VK_NULL_HANDLE;
		DescriptorSetLayout* pLayout = nullptr;
	};
}