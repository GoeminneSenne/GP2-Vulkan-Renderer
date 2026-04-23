#pragma once

namespace dae
{
	struct Buffer final
	{
		VkBuffer buffer = VK_NULL_HANDLE;
		VmaAllocation allocation = nullptr;

		void Destroy(VmaAllocator allocator)
		{
			if (buffer != VK_NULL_HANDLE && allocation != nullptr)
			{
				vmaDestroyBuffer(allocator, buffer, allocation);
			}
		}
	};
}