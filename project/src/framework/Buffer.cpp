#include "Buffer.h"

void dae::Buffer::Destroy(const Context& context) const
{
	if (handle != VK_NULL_HANDLE && allocation != nullptr)
	{
		vmaDestroyBuffer(context.allocator, handle, allocation);
	}
}

void dae::UniformBuffer::Destroy(const Context& context) const
{
	vmaUnmapMemory(context.allocator, allocation);
	Buffer::Destroy(context);
}
