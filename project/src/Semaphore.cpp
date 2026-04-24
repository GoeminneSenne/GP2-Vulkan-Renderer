#include "Semaphore.h"

#include "Common.h"
#include "Context.h"

void dae::Semaphore::Create(const Context& context)
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	LOG_VULKAN_ERROR(
		vkCreateSemaphore(context.logicalDevice.device, &semaphoreInfo, nullptr, &m_semaphore),
		"failed to create semaphore"
	);
}

void dae::Semaphore::Destroy(const Context& context) const
{
	vkDestroySemaphore(context.logicalDevice.device, m_semaphore, nullptr);
}

VkSemaphore dae::Semaphore::Get() const
{
	return m_semaphore;
}
