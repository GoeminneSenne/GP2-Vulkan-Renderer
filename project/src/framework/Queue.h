#pragma once

#include <vulkan/vulkan_core.h>
#include "Fence.h"

namespace dae
{
	class Fence;

	class Queue final
	{
	public:
		Queue() = default;
		Queue(VkQueue queue, uint32_t family);
		
		void Submit(const VkSubmitInfo& submitInfo, const Fence& fence = {}) const;
		VkResult Present(const VkPresentInfoKHR& presentInfo) const;

		VkQueue Get() const;
		uint32_t GetFamily() const;

	private:
		VkQueue m_queue = VK_NULL_HANDLE;
		uint32_t m_family{};
	};
}