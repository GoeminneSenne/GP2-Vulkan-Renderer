#include "Buffer.h"

#include "Common.h"

void dae::Buffer::Destroy(const Context& context) const
{
	if (handle != VK_NULL_HANDLE && allocation != nullptr)
	{
		vmaDestroyBuffer(context.allocator, handle, allocation);
	}
}
