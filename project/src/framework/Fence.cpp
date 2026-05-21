#include "Fence.h"

#include "Common.h"
#include "LogicalDevice.h"
#include "Context.h"

void dae::Fence::Create(const Context& context)
{
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	LOG_VULKAN_ERROR(vkCreateFence(context.logicalDevice.device, &fenceInfo, nullptr, &m_fence),
		"failed to create fence!"
	);
}

void dae::Fence::Destroy(const Context& context) const
{
	vkDestroyFence(context.logicalDevice.device, m_fence, nullptr);
}

void dae::Fence::Wait(const Context& context) const
{
	vkWaitForFences(context.logicalDevice.device, 1, &m_fence, VK_TRUE, UINT64_MAX);
}

void dae::Fence::Reset(const Context& context) const
{
	vkResetFences(context.logicalDevice.device, 1, &m_fence);
}

VkFence dae::Fence::Get() const
{
	return m_fence;
}


