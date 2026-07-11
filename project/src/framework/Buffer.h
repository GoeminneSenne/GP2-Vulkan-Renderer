#pragma once

#include <vk_mem_alloc.h>

#include "Context.h"

namespace dae
{
	struct Buffer final
	{
		//TODO allocationInfo als member toevoegen?
		VkBuffer handle = VK_NULL_HANDLE;
		VmaAllocation allocation = nullptr;
		VkDeviceSize size = 0;


		void Create(const Context& context, VkDeviceSize size, VkBufferUsageFlags usage,
			VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags allocFlags);
		void Destroy(const Context& context) const;

		void CreateStaging(const Context& context, VkDeviceSize size);
		void CreateVertex(const Context& context, VkDeviceSize size);
		void CreateIndex(const Context& context, VkDeviceSize size);
		void CreateUniform(const Context& context, VkDeviceSize size);

		void UploadData(const Context& context, const void* data, VkDeviceSize size);
	};
}
