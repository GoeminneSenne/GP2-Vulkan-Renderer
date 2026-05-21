#pragma once

#include <vulkan/vulkan_core.h>
 
#include "CommandBuffer.h"
#include "Context.h"

namespace dae
{
	namespace BufferHelpers
	{
		
		inline void CreateBuffer(const Context& context, VkDeviceSize size, VkBufferUsageFlags usage,
			VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags allocFlags,
			Buffer& buffer)
		{
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo allocInfo{};
			allocInfo.usage = memoryUsage;
			allocInfo.flags = allocFlags;

			LOG_VULKAN_ERROR(
				vmaCreateBuffer(context.allocator, &bufferInfo, &allocInfo, &buffer.buffer, &buffer.allocation, nullptr),
				"failed to allocate buffer!"
			);

			buffer.size = size;
		}

		//Specific Create Helpers
		inline void CreateStaging(const Context& context, VkDeviceSize size, Buffer& buffer)
		{
			CreateBuffer(context, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO,
				VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
				buffer);
		}

		inline void CreateVertex(const Context& context, VkDeviceSize size, Buffer& buffer)
		{
			CreateBuffer(context, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VMA_MEMORY_USAGE_AUTO, 0, buffer);
		}

		inline void CreateIndex(const Context& context, VkDeviceSize size, Buffer& buffer)
		{
			CreateBuffer(context, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VMA_MEMORY_USAGE_AUTO, 0, buffer);
		}

		inline void CreateUniform(const Context& context, VkDeviceSize size, Buffer& buffer)
		{
			CreateBuffer(context, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO,
				VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
				buffer);
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