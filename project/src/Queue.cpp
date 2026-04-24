#include "Queue.h"

#include "Common.h"
#include "Fence.h"

dae::Queue::Queue(VkQueue queue, uint32_t family)
	: m_queue(queue), m_family(family)
{}

void dae::Queue::Submit(const VkSubmitInfo& submitInfo, const Fence& fence) const
{
	LOG_VULKAN_ERROR(
		vkQueueSubmit(m_queue, 1, &submitInfo, fence.Get()),
		"failed to submit draw command buffer!"
	);
}

VkResult dae::Queue::Present(const VkPresentInfoKHR& presentInfo) const
{
	return vkQueuePresentKHR(m_queue, &presentInfo);
}

VkQueue dae::Queue::Get() const
{
	return m_queue;
}

uint32_t dae::Queue::GetFamily() const
{
	return m_family;
}
