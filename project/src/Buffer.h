#pragma once

#include <vk_mem_alloc.h>

#include "Context.h"

namespace dae
{
	struct Buffer final
	{
		//TODO allocationInfo als member toevoegen?
		VkBuffer buffer = VK_NULL_HANDLE;
		VmaAllocation allocation = nullptr;
		VkDeviceSize size = 0;

		void Destroy(const Context& context) const
		{
			if (buffer != VK_NULL_HANDLE && allocation != nullptr)
			{
				vmaDestroyBuffer(context.allocator, buffer, allocation);
			}
		}
	};
}
