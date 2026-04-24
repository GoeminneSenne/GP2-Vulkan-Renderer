#pragma once

#include <vulkan/vulkan_core.h>

namespace dae
{
	struct LogicalDevice;

	class Fence final
	{
	public:
		void Create(const LogicalDevice& logicalDevice);
		void Destroy(const LogicalDevice& logicalDevice) const;
		void Wait(const LogicalDevice& logicalDevice) const;
		void Reset(const LogicalDevice& logicalDevice) const;
		
		VkFence Get() const;

	private:
		VkFence m_fence = VK_NULL_HANDLE;
	};
}