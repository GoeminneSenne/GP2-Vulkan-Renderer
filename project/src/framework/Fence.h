#pragma once

#include <vulkan/vulkan_core.h>

namespace dae
{
	struct Context;

	class Fence final
	{
	public:
		Fence() = default;

		void Create(const Context& context);
		void Destroy(const Context& context) const;
		void Wait(const Context& context) const;
		void Reset(const Context& context) const;
		
		VkFence GetHandle() const;

	private:
		VkFence m_handle = VK_NULL_HANDLE;
	};
}