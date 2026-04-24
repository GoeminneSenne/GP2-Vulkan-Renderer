#pragma once

#include <vulkan/vulkan_core.h>

namespace dae
{
	struct LogicalDevice;

	class Semaphore final
	{
	public:
		void Create(const LogicalDevice& logicalDevice);
		void Destroy(const LogicalDevice& logicalDevice) const;
		
		VkSemaphore Get() const;

	private:
		VkSemaphore m_semaphore = VK_NULL_HANDLE;
	};
}