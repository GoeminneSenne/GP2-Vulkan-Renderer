#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

namespace dae
{
	struct Context;

	class Semaphore final
	{
	public:
		void Create(const Context& context);
		void Destroy(const Context& context) const;

		VkSemaphore GetHandle() const;

	private:
		VkSemaphore m_handle = VK_NULL_HANDLE;
	};

	class Fence final
	{
	public:
		Fence() = default;

		void Create(const Context& context);
		void Destroy(const Context& context) const;
		void Wait(const Context& context) const;
		void Reset(const Context& context) const;

		VkFence GetHandle() const;

	private:
		VkFence m_handle = VK_NULL_HANDLE;
	};

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