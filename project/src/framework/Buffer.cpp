#include "Buffer.h"

#include "Common.h"


void dae::Buffer::Create(const Context& context, VkDeviceSize size, VkBufferUsageFlags usage,
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
		vmaCreateBuffer(context.allocator, &bufferInfo, &allocInfo, &handle, &allocation, nullptr),
		"failed to allocate buffer!"
	);

	this->size = size;
}

void dae::Buffer::Destroy(const Context& context) const
{
	if (handle != VK_NULL_HANDLE && allocation != nullptr)
	{
		vmaDestroyBuffer(context.allocator, handle, allocation);
	}
}

void dae::Buffer::CreateStaging(const Context& context, VkDeviceSize size)
{
	Create(context, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO,
		VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
}

void dae::Buffer::CreateVertex(const Context& context, VkDeviceSize size)
{
	Create(context, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VMA_MEMORY_USAGE_AUTO, 0);
}

void dae::Buffer::CreateIndex(const Context& context, VkDeviceSize size)
{
	Create(context, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VMA_MEMORY_USAGE_AUTO, 0);
}

void dae::Buffer::CreateUniform(const Context& context, VkDeviceSize size)
{
	Create(context, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO,
		VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
}


void dae::Buffer::UploadData(const Context& context, const void* data, VkDeviceSize size)
{
	void* mapped;
	vmaMapMemory(context.allocator, allocation, &mapped);
	std::memcpy(mapped, data, static_cast<size_t>(size));
	vmaUnmapMemory(context.allocator, allocation);
}
