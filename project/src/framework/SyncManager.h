#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

#include "Semaphore.h"
#include "Fence.h"

namespace dae
{
	class SyncManager final
	{
	public:
		void Create(const Context& context, uint32_t swapchainImageCount);
		void Create(const Context& context, uint32_t swapchainImageCount, uint32_t framesInFlight);
		void Destroy(const Context& context);

		void RecreateRenderFinishedSemaphores(const Context& context, uint32_t swapchainImageCount);

		const Semaphore * GetImageAvailableSemaphore(uint32_t frameIndex) const;
		const Fence * GetInFlightFence(uint32_t frameIndex) const;

		const Semaphore * GetRenderFinishedSemaphore(uint32_t imageIndex) const;

		uint32_t GetCurrentFrameIndex() const;
		uint32_t GetFramesInFlight() const;
		void NextFrame();


	private:
		//Per frame sync objects
		std::vector<Semaphore> m_imageAvailableSemaphores{};
		std::vector<Fence> m_inFlightFences{};

		uint32_t m_currentFrame = 0;
		uint32_t m_framesInFlight = 3;
		
		//Per swapchain image resources
		std::vector<Semaphore> m_renderFinishedSemaphores{};

	};
}