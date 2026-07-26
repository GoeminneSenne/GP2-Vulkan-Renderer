#include "Queue.h"

#include "Common.h"

dae::Queue::Queue(VkQueue queue, uint32_t family)
	: m_handle(queue), m_family(family)
{}

void dae::Queue::Submit(const VkSubmitInfo& submitInfo, const Fence& fence) const
{
	LOG_VULKAN_ERROR(
		vkQueueSubmit(m_handle, 1, &submitInfo, fence.GetHandle()),
		"failed to submit draw command buffer!"
	);
}

VkResult dae::Queue::Present(const VkPresentInfoKHR& presentInfo) const
{
	return vkQueuePresentKHR(m_handle, &presentInfo);
}

VkQueue dae::Queue::GetHandle() const
{
	return m_handle;
}

uint32_t dae::Queue::GetFamily() const
{
	return m_family;
}
