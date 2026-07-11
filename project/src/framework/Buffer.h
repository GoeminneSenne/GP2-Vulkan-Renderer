#pragma once

#include <vk_mem_alloc.h>

#include "Context.h"

namespace dae
{
	struct Buffer final
	{
		VkBuffer handle = VK_NULL_HANDLE;
		VmaAllocation allocation = nullptr;
		VkDeviceSize size = 0;
		VmaAllocationInfo allocInfo = {};

		void Destroy(const Context& context) const;
	};
}
