#pragma once

#include <vk_mem_alloc.h>

#include "Buffer.h"
#include "CommandBuffer.h"
#include "Context.h"

namespace dae
{
	namespace BufferHelpers
	{
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
