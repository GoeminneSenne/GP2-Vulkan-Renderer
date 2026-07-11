#pragma once

#include <vk_mem_alloc.h>

#include "Buffer.h"
#include "CommandBuffer.h"
#include "Common.h"
#include "Context.h"

namespace dae
{
	namespace BufferHelpers
	{
		inline void CreateBuffer(const Context& context, Buffer& buffer, VkDeviceSize size, VkBufferUsageFlags usage,
			VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags allocFlags)
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
				vmaCreateBuffer(context.allocator, &bufferInfo, &allocInfo, &buffer.handle, &buffer.allocation, &buffer.allocInfo),
				"failed to allocate buffer!"
			);

			buffer.size = size;
		}

		inline void CreateStaging(const Context& context, Buffer& buffer, VkDeviceSize size)
		{
			CreateBuffer(context,buffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO,
				VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
		}
		inline void CreateVertex(const Context& context, Buffer& buffer, VkDeviceSize size)
		{
			CreateBuffer(context, buffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VMA_MEMORY_USAGE_AUTO, 0);
		}
		inline void CreateIndex(const Context& context, Buffer& buffer, VkDeviceSize size)
		{
			CreateBuffer(context, buffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VMA_MEMORY_USAGE_AUTO, 0);
		}
		inline void CreateUniform(const Context& context, Buffer& buffer, VkDeviceSize size)
		{
			CreateBuffer(context, buffer, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO,
				VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
		}


		inline void CopyBuffer(const CommandBuffer& command, const Buffer& source, const Buffer& dest, VkDeviceSize size)
		{
			VkBufferCopy copyRegion{};
			copyRegion.size = size;
			vkCmdCopyBuffer(command.handle, source.handle, dest.handle, 1, &copyRegion);
		}

		inline void CopyBufferToImage(const CommandBuffer& command, const Buffer& source, const Image& image)
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

			vkCmdCopyBufferToImage(command.handle, source.handle, image.image,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
		}

		inline void UploadData(const Context& context, const Buffer& buffer, const void* data, VkDeviceSize size)
		{
			void* mapped;
			vmaMapMemory(context.allocator, buffer.allocation, &mapped);
			std::memcpy(mapped, data, static_cast<size_t>(size));
			vmaUnmapMemory(context.allocator, buffer.allocation);
		}
	}
}
