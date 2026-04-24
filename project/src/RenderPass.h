#pragma once

#include <vulkan/vulkan_core.h>
#include "Context.h"

namespace dae
{
	struct RenderPass final
	{
		VkRenderPass renderPass = VK_NULL_HANDLE;

		void Destroy(const Context& context) const
		{
			vkDestroyRenderPass(context.logicalDevice.device, renderPass, nullptr);
		}
	};
}