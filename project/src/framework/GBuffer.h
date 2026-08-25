#pragma once

#include "Image.h"
#include "Context.h"
#include "DepthHelpers.h"

namespace dae
{
	struct GBuffer
	{
		Image depthImage{};
		Image albedoImage{};
		Image normalImage{};
		Image metallicRoughnessImage{};

		void Create(const Context& context, const Swapchain& swapchain)
		{
			ImageBuilder imageBuilder{};

			// --- Albedo Image ----------------------
			imageBuilder
				.SetWidth(swapchain.extent.width)
				.SetHeight(swapchain.extent.height)
				.SetMipLevels(1)
				.SetFormat(VK_FORMAT_R8G8B8A8_UNORM)
				.SetTiling(VK_IMAGE_TILING_OPTIMAL)
				.SetUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
				.SetAspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
				.SetAllocUsage(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE)
				.Build(context, albedoImage);


			// --- Normal Image ----------------------
			imageBuilder = {};
			imageBuilder
				.SetWidth(swapchain.extent.width)
				.SetHeight(swapchain.extent.height)
				.SetMipLevels(1)
				.SetFormat(VK_FORMAT_R16G16B16A16_SFLOAT)
				.SetTiling(VK_IMAGE_TILING_OPTIMAL)
				.SetUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
				.SetAspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
				.SetAllocUsage(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE)
				.Build(context, normalImage);

			// --- Depth Image ----------------------
			DepthHelpers::CreateDepthImage(context, swapchain, depthImage);

			// --- Metallic & Roughness Image ----------------------
			imageBuilder = {};
			imageBuilder
				.SetWidth(swapchain.extent.width)
				.SetHeight(swapchain.extent.height)
				.SetMipLevels(1)
				.SetFormat(VK_FORMAT_R8G8B8A8_UNORM)
				.SetTiling(VK_IMAGE_TILING_OPTIMAL)
				.SetUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
				.SetAspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
				.SetAllocUsage(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE)
				.Build(context, metallicRoughnessImage);
		}

		void Destroy(const Context& context)
		{
			depthImage.Destroy(context);
			albedoImage.Destroy(context);
			normalImage.Destroy(context);
			metallicRoughnessImage.Destroy(context);
		}
	};
}