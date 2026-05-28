#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

#include "Framebuffer.h"
#include "Image.h"

namespace dae
{
	struct Instance;
	struct Window;
	struct PhysicalDevice;
	struct LogicalDevice;

	struct Swapchain
	{
		void Create(const Window& window, const Context& context);
		void Destroy(const Context& context);

		VkSwapchainKHR handle = VK_NULL_HANDLE;
		std::vector<Image> images;
		VkFormat imageFormat;
		VkExtent2D extent;
		std::vector<Framebuffer> framebuffers;
	};
}
