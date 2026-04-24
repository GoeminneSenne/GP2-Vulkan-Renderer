#pragma once

#include <vulkan/vulkan_core.h>
#include "LogicalDevice.h"

namespace dae
{
	struct RenderPass final
	{
		VkRenderPass renderPass = VK_NULL_HANDLE;

		void Destroy(const LogicalDevice& logicalDevice) const
		{
			vkDestroyRenderPass(logicalDevice.device, renderPass, nullptr);
		}
	};
}