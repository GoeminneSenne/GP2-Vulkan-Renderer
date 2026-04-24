#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>
#include "LogicalDevice.h"

namespace dae
{
	struct Framebuffer final
	{
		VkFramebuffer framebuffer;
		VkExtent2D extent;
		std::vector<VkImageView> attachments;

		void Destroy(const LogicalDevice& logicalDevice) const
		{
			vkDestroyFramebuffer(logicalDevice.device, framebuffer, nullptr);
		}
	};
}