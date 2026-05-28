#include "CommandBuffer.h"

#include "Common.h"
#include "LogicalDevice.h"

///CommandBuffer
///////////////////////////////////////////
void dae::CommandBuffer::Begin(VkCommandBufferUsageFlags flags)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = flags;

	vkBeginCommandBuffer(handle, &beginInfo);
}

void dae::CommandBuffer::End()
{
	vkEndCommandBuffer(handle);
}

///CommandPool
///////////////////////////////////////////

void dae::CommandPool::CreateCommandPool(LogicalDevice* pLogicalDevice)
{
	m_pLogicalDevice = pLogicalDevice;

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = pLogicalDevice->graphicsQueue.GetFamily();

	LOG_VULKAN_ERROR(
		vkCreateCommandPool(pLogicalDevice->handle, &poolInfo, nullptr, &m_commandPool),
		"failed to create command pool!"
	);
}

void dae::CommandPool::Destroy()
{
	vkDestroyCommandPool(m_pLogicalDevice->handle, m_commandPool, nullptr);
}

dae::CommandBuffer dae::CommandPool::Allocate() const
{
	CommandBuffer commandBuffer{};

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	LOG_VULKAN_ERROR(
		vkAllocateCommandBuffers(m_pLogicalDevice->handle, &allocInfo, &commandBuffer.handle),
		"failed to allocate command buffer!"
	);

	return commandBuffer;
}

std::vector<dae::CommandBuffer> dae::CommandPool::Allocate(uint32_t count) const
{
	std::vector<VkCommandBuffer> rawBuffers;
	rawBuffers.resize(count);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = count;

	LOG_VULKAN_ERROR(
		vkAllocateCommandBuffers(m_pLogicalDevice->handle, &allocInfo, rawBuffers.data()),
		"failed to allocate command buffer!"
	);

	std::vector<CommandBuffer> commandBuffers;
	commandBuffers.reserve(count);
	for (auto& rawBuffer : rawBuffers)
	{
		commandBuffers.emplace_back(CommandBuffer{ rawBuffer });
	}
	return commandBuffers;
}

void dae::CommandPool::Free(CommandBuffer& commandBuffer) const
{
	vkFreeCommandBuffers(m_pLogicalDevice->handle, m_commandPool, 1, &commandBuffer.handle);
}

void dae::CommandPool::Free(std::vector<CommandBuffer>& commandBuffers) const
{
	for (auto& commandBuffer : commandBuffers)
	{
		Free(commandBuffer);
	}
}

dae::CommandBuffer dae::CommandPool::BeginSingleTimeCommand() const
{
	CommandBuffer commandBuffer{};

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_commandPool;
	allocInfo.commandBufferCount = 1;

	LOG_VULKAN_ERROR(
		vkAllocateCommandBuffers(m_pLogicalDevice->handle, &allocInfo, &commandBuffer.handle),
		"failed to allocate command buffer!"
	);

	commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	return commandBuffer;
}

void dae::CommandPool::EndSingleTimeCommand(CommandBuffer& commandBuffer) const
{
	commandBuffer.End();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer.handle;

	m_pLogicalDevice->graphicsQueue.Submit(submitInfo);
	vkQueueWaitIdle(m_pLogicalDevice->graphicsQueue.GetHandle());

	vkFreeCommandBuffers(m_pLogicalDevice->handle, m_commandPool, 1, &commandBuffer.handle);
}