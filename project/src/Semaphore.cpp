#include "Semaphore.h"

#include "Common.h"
#include "LogicalDevice.h"

void dae::Semaphore::Create(const LogicalDevice& logicalDevice)
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	LOG_VULKAN_ERROR(
		vkCreateSemaphore(logicalDevice.device, &semaphoreInfo, nullptr, &m_semaphore),
		"failed to create semaphore"
	);
}

void dae::Semaphore::Destroy(const LogicalDevice& logicalDevice) const
{
	vkDestroySemaphore(logicalDevice.device, m_semaphore, nullptr);
}

VkSemaphore dae::Semaphore::Get() const
{
	return m_semaphore;
}
