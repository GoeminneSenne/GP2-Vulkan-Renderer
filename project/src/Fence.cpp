#include "Fence.h"

#include "Common.h"
#include "LogicalDevice.h"

void dae::Fence::Create(const LogicalDevice& logicalDevice)
{
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	LOG_VULKAN_ERROR(vkCreateFence(logicalDevice.device, &fenceInfo, nullptr, &m_fence),
		"failed to create fence!"
	);
}

void dae::Fence::Destroy(const LogicalDevice& logicalDevice) const
{
	vkDestroyFence(logicalDevice.device, m_fence, nullptr);
}

void dae::Fence::Wait(const LogicalDevice& logicalDevice) const
{
	vkWaitForFences(logicalDevice.device, 1, &m_fence, VK_TRUE, UINT64_MAX);
}

void dae::Fence::Reset(const LogicalDevice& logicalDevice) const
{
	vkResetFences(logicalDevice.device, 1, &m_fence);
}

VkFence dae::Fence::Get() const
{
	return m_fence;
}


