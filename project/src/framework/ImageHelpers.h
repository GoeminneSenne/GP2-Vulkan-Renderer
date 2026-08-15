#pragma once

#include <cassert>
#include <stdexcept>
#include <filesystem>

#include <vulkan/vulkan_core.h>

#include "../../inc/stb_image.h"

#include "Buffer.h"
#include "BufferHelpers.h"
#include "CommandBuffer.h"
#include "Common.h"
#include "Context.h"
#include "Image.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"

namespace dae
{
	namespace ImageHelpers
	{
		inline void CreateImageView(const Context& context, Image& image)
		{
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image.image;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = image.format;
			viewInfo.subresourceRange.aspectMask = image.aspectMask;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = image.mipLevels;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			LOG_VULKAN_ERROR(
				vkCreateImageView(context.logicalDevice.handle, &viewInfo, nullptr, &image.view),
				"failed to create image view!"
			);
		}

		inline void GenerateMipmaps(const CommandBuffer& commandBuffer, const Context& context, Image& image)
		{
			VkFormatProperties formatProperties;
			vkGetPhysicalDeviceFormatProperties(context.physicalDevice.handle, image.format, &formatProperties);

			if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
				throw std::runtime_error("texture image format does not support linear blitting!");
			}

			VkImageMemoryBarrier2 barrier2{};
			barrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
			barrier2.image = image.image;
			barrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier2.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier2.subresourceRange.baseArrayLayer = 0;
			barrier2.subresourceRange.layerCount = 1;
			barrier2.subresourceRange.levelCount = 1;

			int32_t mipWidth = image.extent.width;
			int32_t mipHeight = image.extent.height;

			for (uint32_t i = 1; i < image.mipLevels; i++) {

				barrier2.subresourceRange.baseMipLevel = i - 1;
				barrier2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier2.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier2.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
				barrier2.dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
				barrier2.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
				barrier2.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;

				VkDependencyInfo dependencyInfo{};
				dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
				dependencyInfo.imageMemoryBarrierCount = 1;
				dependencyInfo.pImageMemoryBarriers = &barrier2;

				vkCmdPipelineBarrier2(commandBuffer.handle, &dependencyInfo);

				VkImageBlit2 blit2{};
				blit2.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
				blit2.srcOffsets[0] = { 0, 0, 0 };
				blit2.srcOffsets[1] = { mipWidth, mipHeight, 1 };
				blit2.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit2.srcSubresource.mipLevel = i - 1;
				blit2.srcSubresource.baseArrayLayer = 0;
				blit2.srcSubresource.layerCount = 1;
				blit2.dstOffsets[0] = { 0, 0, 0 };
				blit2.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
				blit2.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit2.dstSubresource.mipLevel = i;
				blit2.dstSubresource.baseArrayLayer = 0;
				blit2.dstSubresource.layerCount = 1;

				VkBlitImageInfo2 blitInfo2{};
				blitInfo2.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
				blitInfo2.srcImage = image.image;
				blitInfo2.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				blitInfo2.dstImage = image.image;
				blitInfo2.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				blitInfo2.regionCount = 1;
				blitInfo2.pRegions = &blit2;
				blitInfo2.filter = VK_FILTER_LINEAR;

				vkCmdBlitImage2(commandBuffer.handle, &blitInfo2);

				barrier2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier2.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier2.srcAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
				barrier2.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
				barrier2.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
				barrier2.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;

				vkCmdPipelineBarrier2(commandBuffer.handle, &dependencyInfo);

				if (mipWidth > 1) mipWidth /= 2;
				if (mipHeight > 1) mipHeight /= 2;
			}

			barrier2.subresourceRange.baseMipLevel = image.mipLevels - 1;
			barrier2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier2.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier2.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
			barrier2.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
			barrier2.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
			barrier2.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;

			VkDependencyInfo dependencyInfo{};
			dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
			dependencyInfo.imageMemoryBarrierCount = 1;
			dependencyInfo.pImageMemoryBarriers = &barrier2;

			vkCmdPipelineBarrier2(commandBuffer.handle, &dependencyInfo);
		}

		inline void TransitionImageLayout(const CommandBuffer& commandBuffer, Image& image, VkImageLayout newLayout,
			VkPipelineStageFlags2 sourceStage, VkPipelineStageFlags2 destinationStage,
			VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask)
		{
			VkImageMemoryBarrier2 barrier2{};
			barrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
			barrier2.srcStageMask = sourceStage;
			barrier2.srcAccessMask = srcAccessMask;
			barrier2.dstStageMask = destinationStage;
			barrier2.dstAccessMask = dstAccessMask;
			barrier2.oldLayout = image.layout;
			barrier2.newLayout = newLayout;
			barrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier2.image = image.image;
			barrier2.subresourceRange.aspectMask = image.aspectMask;
			barrier2.subresourceRange.baseMipLevel = 0;
			barrier2.subresourceRange.levelCount = image.mipLevels;
			barrier2.subresourceRange.baseArrayLayer = 0;
			barrier2.subresourceRange.layerCount = 1;

			VkDependencyInfo dependencyInfo{};
			dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
			dependencyInfo.imageMemoryBarrierCount = 1;
			dependencyInfo.pImageMemoryBarriers = &barrier2;

			vkCmdPipelineBarrier2(commandBuffer.handle, &dependencyInfo);

			image.layout = newLayout;
		}


		inline void CreateTexture(const Context& context, const std::string& path, VkFormat format, Image& image)
		{
			int texWidth, texHeight, texChannels;
			VkDeviceSize imageSize;
		
			stbi_uc* pixels8 = nullptr;
			stbi_us* pixels16 = nullptr;

			if (format == VK_FORMAT_R16G16B16A16_UNORM)
			{
				pixels16 = stbi_load_16(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
				imageSize = texWidth * texHeight * 8;

				if (!pixels16)
				{
					throw std::runtime_error("failed to load image: " + path);
				}
			}
			else
			{
				pixels8 = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
				imageSize = texWidth * texHeight * 4;

				if (!pixels8)
				{
					throw std::runtime_error("failed to load image: " + path);
				}
			}

			Buffer stagingBuffer;
			BufferHelpers::CreateStaging(context, stagingBuffer, imageSize);
			
			if (pixels8)
			{
				BufferHelpers::UploadData(context, stagingBuffer, pixels8, imageSize);
				stbi_image_free(pixels8);
			}
			else
			{
				BufferHelpers::UploadData(context, stagingBuffer, pixels16, imageSize);
				stbi_image_free(pixels16);
			}


			uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

			ImageBuilder imageBuilder = {};
			imageBuilder
				.SetWidth(texWidth)
				.SetHeight(texHeight)
				.SetMipLevels(mipLevels)
				.SetFormat(format)
				.SetTiling(VK_IMAGE_TILING_OPTIMAL)
				.SetUsageFlags(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
				.SetAspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
				.SetAllocUsage(VMA_MEMORY_USAGE_AUTO)
				.SetAllocFlags(VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT)
				.Build(context, image);

			CommandBuffer commandBuffer = context.commandPool.BeginSingleTimeCommand();
			TransitionImageLayout(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
				VK_ACCESS_2_NONE, VK_ACCESS_2_TRANSFER_WRITE_BIT);
			context.commandPool.EndSingleTimeCommand(commandBuffer);

			commandBuffer = context.commandPool.BeginSingleTimeCommand();
			BufferHelpers::CopyBufferToImage(commandBuffer, stagingBuffer, image);
			context.commandPool.EndSingleTimeCommand(commandBuffer);

			stagingBuffer.Destroy(context);

			commandBuffer = context.commandPool.BeginSingleTimeCommand();
			GenerateMipmaps(commandBuffer, context, image);
			context.commandPool.EndSingleTimeCommand(commandBuffer);
		}

		//Filesystem overload
		inline void CreateTexture(const Context& context, const std::filesystem::path& path, VkFormat format, Image& image)
		{
			CreateTexture(context, path.generic_string(), format, image);
		}

		//Overload without VkFormat to be backwards compatible with VulkanDemo app
		inline void CreateTexture(const Context& context, const std::string& path, Image& image)
		{
			return CreateTexture(context, path, VK_FORMAT_R8G8B8A8_SRGB, image);
		}

	}
}
