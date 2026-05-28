#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace dae
{
	struct LogicalDevice;

	struct CommandBuffer final
	{
		VkCommandBuffer handle = VK_NULL_HANDLE;

		void Begin(VkCommandBufferUsageFlags flags = {});
		void End();
	};

	class CommandPool final
	{
	public:
		void CreateCommandPool(LogicalDevice* pLogicalDevice);
		void Destroy();

		CommandBuffer Allocate() const;
		std::vector<CommandBuffer> Allocate(uint32_t count) const;

		void Free(CommandBuffer& commandBuffer) const;
		void Free(std::vector<CommandBuffer>& commandBuffers) const;

		CommandBuffer BeginSingleTimeCommand() const;
		void EndSingleTimeCommand(CommandBuffer& commandBuffer) const;

	private:
		VkCommandPool m_commandPool = VK_NULL_HANDLE;
		LogicalDevice* m_pLogicalDevice = nullptr;
	};
}