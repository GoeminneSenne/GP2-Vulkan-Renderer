#include "Semaphore.h"

#include "Common.h"
#include "Context.h"

void dae::Semaphore::Create(const Context& context)
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	LOG_VULKAN_ERROR(
		vkCreateSemaphore(context.logicalDevice.handle, &semaphoreInfo, nullptr, &m_handle),
		"failed to create semaphore"
	);
}

void dae::Semaphore::Destroy(const Context& context) const
{
	if (m_handle == VK_NULL_HANDLE) return;

	vkDestroySemaphore(context.logicalDevice.handle, m_handle, nullptr);
}

VkSemaphore dae::Semaphore::GetHandle() const
{
	return m_handle;
}
