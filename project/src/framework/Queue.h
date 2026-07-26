#pragma once

#include <vulkan/vulkan_core.h>
#include "SyncManager.h"

namespace dae
{
	class Queue final
	{
	public:
		Queue() = default;
		Queue(VkQueue queue, uint32_t family);
		
		void Submit(const VkSubmitInfo& submitInfo, const Fence& fence = {}) const;
		VkResult Present(const VkPresentInfoKHR& presentInfo) const;

		VkQueue GetHandle() const;
		uint32_t GetFamily() const;

	private:
		VkQueue m_handle = VK_NULL_HANDLE;
		uint32_t m_family{};
	};
}