#pragma once

#include <vk_mem_alloc.h>

namespace dae
{
	struct Buffer final
	{
		//TODO allocationInfo als member toevoegen?
		VkBuffer buffer = VK_NULL_HANDLE;
		VmaAllocation allocation = nullptr;
		VkDeviceSize size = 0;

		void Destroy(VmaAllocator allocator) const
		{
			if (buffer != VK_NULL_HANDLE && allocation != nullptr)
			{
				vmaDestroyBuffer(allocator, buffer, allocation);
			}
		}
	};
}