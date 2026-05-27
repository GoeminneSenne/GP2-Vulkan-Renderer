#include "SyncManager.h"

#include <algorithm>

#include "Common.h"

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
