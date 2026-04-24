#pragma once

#include <vulkan/vulkan_core.h>

#include "FramebufferBuilder.h"
#include "ImageHelpers.h"
#include "LogicalDevice.h"

namespace dae
{
	namespace SwapchainHelpers
	{
		inline Image WrapSwapchainImage(const LogicalDevice& logicalDevice,
			VkImage swapchainImage, VkFormat format, VkExtent2D extent)
		{
			Image image;
			image.image = swapchainImage;
			image.format = format;
			image.extent = extent;
			image.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			image.mipLevels = 1;

			ImageHelpers::CreateImageView(logicalDevice, image);

			return image;
		}

		inline void CreateFramebuffers(const LogicalDevice& logicalDevice, const VkRenderPass& renderPass, const Image& depthImage, Swapchain& swapchain)
		{
			swapchain.framebuffers.resize(swapchain.images.size());

			for (size_t idx{}; idx < swapchain.images.size(); ++idx)
			{
				std::vector<VkImageView> attachments = { swapchain.images[idx].view, depthImage.view };

				FramebufferBuilder framebufferBuilder = {};
				framebufferBuilder
					.SetRenderPass(renderPass)
					.SetAttachments(attachments)
					.SetWidth(swapchain.extent.width)
					.SetHeight(swapchain.extent.height)
					.Build(logicalDevice, swapchain.framebuffers[idx]);
			}
		}
	}
}