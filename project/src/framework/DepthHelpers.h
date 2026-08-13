#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>

#include "Context.h"

namespace dae::DepthHelpers
{
	inline VkFormat FindSupportedFormat(const Context& context, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (VkFormat format : candidates)
		{
			VkFormatProperties properties;
			vkGetPhysicalDeviceFormatProperties(context.physicalDevice.handle, format, &properties);

			if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}

		throw std::runtime_error("failed to find supported format!");
	}

	inline VkFormat FindDepthFormat(const Context& context)
	{
		return FindSupportedFormat(
			context,
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	inline bool HasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	inline void CreateDepthImage(const Context& context, const Swapchain& swapchain, Image& depthImage)
	{
		VkFormat depthFormat = FindDepthFormat(context);

		VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (DepthHelpers::HasStencilComponent(depthFormat))
		{
			aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}

		ImageBuilder imageBuilder = {};
		imageBuilder
			.SetWidth(swapchain.extent.width)
			.SetHeight(swapchain.extent.height)
			.SetMipLevels(1)
			.SetFormat(depthFormat)
			.SetTiling(VK_IMAGE_TILING_OPTIMAL)
			.SetUsageFlags(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
			.SetAspectMask(aspectMask)
			.SetAllocUsage(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE)
			.SetAllocFlags(0)
			.Build(context, depthImage);

		CommandBuffer commandBuffer = context.commandPool.BeginSingleTimeCommand();
		ImageHelpers::TransitionImageLayout(commandBuffer, depthImage, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
			VK_ACCESS_2_NONE, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
		context.commandPool.EndSingleTimeCommand(commandBuffer);
	}
}