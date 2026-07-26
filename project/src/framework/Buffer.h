#pragma once

#include <vk_mem_alloc.h>

#include "Context.h"

namespace dae
{
	struct Buffer
	{
		VkBuffer handle = VK_NULL_HANDLE;
		VmaAllocation allocation = nullptr;
		VkDeviceSize size = 0;
		VmaAllocationInfo allocInfo = {};

		virtual ~Buffer() = default;
		virtual void Destroy(const Context& context) const;
	};

	struct UniformBuffer : public Buffer
	{
		void* mappedData{};

		virtual void Destroy(const Context& context) const override;
	};
}
