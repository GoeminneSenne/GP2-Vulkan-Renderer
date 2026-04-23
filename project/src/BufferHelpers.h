#pragma once

#include <vulkan/vulkan_core.h>
 
#include "CommandBuffer.h"

namespace dae
{
	namespace BufferHelpers
	{
		//TODO misschien toch BufferBuilder maken
		inline void CreateBuffer(const VmaAllocator& allocator, VkDeviceSize size, VkBufferUsageFlags usage, 
			VmaMemoryUsage allocUsage, VmaAllocationCreateFlags allocFlags,
			Buffer& buffer)
		{
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo allocInfo{};
			allocInfo.usage = allocUsage;
			allocInfo.flags = allocFlags;

			LOG_VULKAN_ERROR(
				vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer.buffer, &buffer.allocation, nullptr),
				"failed to allocate buffer!"
			);

			buffer.size = size;
		}

		inline void CopyBuffer(const CommandBuffer& commandBuffer, const Buffer& srcBuffer, const Buffer& dstBuffer, VkDeviceSize size)
		{
			VkBufferCopy copyRegion{};
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer.commandBuffer, srcBuffer.buffer, dstBuffer.buffer, 1, &copyRegion);
		}

		inline void CopyBufferToImage(const CommandBuffer& commandBuffer, const Buffer& buffer, const Image& image)
		{
			VkBufferImageCopy region{};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;

			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;

			region.imageOffset = { 0, 0, 0 };
			region.imageExtent = { image.extent.width, image.extent.height, 1 };

			vkCmdCopyBufferToImage(commandBuffer.commandBuffer, buffer.buffer, image.image,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		}
	}
}