#pragma once

#include <vulkan/vulkan_core.h>

namespace dae
{
	struct Context;

	class Semaphore final
	{
	public:
		void Create(const Context& context);
		void Destroy(const Context& context) const;
		
		VkSemaphore Get() const;

	private:
		VkSemaphore m_semaphore = VK_NULL_HANDLE;
	};
}