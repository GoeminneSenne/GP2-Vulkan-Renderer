#include "Fence.h"

#include "Common.h"
#include "LogicalDevice.h"
#include "Context.h"

void dae::Fence::Create(const Context& context)
{
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	LOG_VULKAN_ERROR(vkCreateFence(context.logicalDevice.handle, &fenceInfo, nullptr, &m_handle),
		"failed to create fence!"
	);
}

void dae::Fence::Destroy(const Context& context) const
{
	vkDestroyFence(context.logicalDevice.handle, m_handle, nullptr);
}

void dae::Fence::Wait(const Context& context) const
{
	vkWaitForFences(context.logicalDevice.handle, 1, &m_handle, VK_TRUE, UINT64_MAX);
}

void dae::Fence::Reset(const Context& context) const
{
	vkResetFences(context.logicalDevice.handle, 1, &m_handle);
}

VkFence dae::Fence::GetHandle() const
{
	return m_handle;
}


