#include "SyncManager.h"

#include <algorithm>

#include "Common.h"
#include "LogicalDevice.h"
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


void dae::SyncManager::Create(const Context& context, uint32_t swapchainImageCount)
{
	Create(context, swapchainImageCount, MAX_FRAMES_IN_FLIGHT);
}

void dae::SyncManager::Create(const Context& context, uint32_t swapchainImageCount, uint32_t framesInFlight)
{
	//Clamp frames in flight between [1, MAX_FRAMES_IN_FLIGHT]
	framesInFlight = std::clamp<std::uint32_t>(framesInFlight, 1, MAX_FRAMES_IN_FLIGHT);
	m_framesInFlight = framesInFlight;

	//Vector resizing
	m_imageAvailableSemaphores.resize(m_framesInFlight);
	m_renderFinishedSemaphores.resize(m_framesInFlight);
	m_inFlightFences.resize(m_framesInFlight);

	//Create Info's
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	//Per frame Sync objects creation
	for (size_t idx{}; idx < m_framesInFlight; ++idx)
	{
		m_imageAvailableSemaphores[idx].Create(context);
		m_inFlightFences[idx].Create(context);
	}

	//Per image sync objects creation
	for (size_t idx{}; idx < swapchainImageCount; ++idx)
	{
		m_renderFinishedSemaphores[idx].Create(context);
	}
}

void dae::SyncManager::Destroy(const Context& context)
{
	//Per frame objects
	for (size_t idx{}; idx < m_framesInFlight; ++idx)
	{
		m_imageAvailableSemaphores[idx].Destroy(context);
		m_inFlightFences[idx].Destroy(context);
	}

	//Per image objects
	for (auto& semaphore : m_renderFinishedSemaphores)
	{
		semaphore.Destroy(context);
	}
}

void dae::SyncManager::RecreateRenderFinishedSemaphores(const Context& context, uint32_t swapchainImageCount)
{
	for (auto& semaphore : m_renderFinishedSemaphores)
	{
		semaphore.Destroy(context);
	}

	for (size_t idx{}; idx < swapchainImageCount; ++idx)
	{
		m_renderFinishedSemaphores[idx].Create(context);
	}
}

const dae::Semaphore * dae::SyncManager::GetImageAvailableSemaphore(uint32_t frameIndex) const
{
	return &m_imageAvailableSemaphores[frameIndex];
}

const dae::Semaphore* dae::SyncManager::GetRenderFinishedSemaphore(uint32_t imageIndex) const
{
	return &m_renderFinishedSemaphores[imageIndex];
}

const dae::Fence * dae::SyncManager::GetInFlightFence(uint32_t frameIndex) const
{
	return &m_inFlightFences[frameIndex];
}

uint32_t dae::SyncManager::GetCurrentFrameIndex() const
{
	return m_currentFrame;
}

uint32_t dae::SyncManager::GetFramesInFlight() const
{
	return m_framesInFlight;
}

void dae::SyncManager::NextFrame()
{
	m_currentFrame = (m_currentFrame + 1) % m_framesInFlight;
}
