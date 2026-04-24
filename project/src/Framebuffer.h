#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

#include "Context.h"
#include "LogicalDevice.h"

namespace dae
{
	struct Framebuffer final
	{
		VkFramebuffer framebuffer;
		VkExtent2D extent;
		std::vector<VkImageView> attachments;

		void Destroy(const Context& context) const
		{
			vkDestroyFramebuffer(context.logicalDevice.device, framebuffer, nullptr);
		}
	};
}