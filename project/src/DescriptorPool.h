#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace dae
{
	struct DescriptorSet;
	struct DescriptorSetLayout;
	class LogicalDevice;

	class DescriptorPool final
	{
	public:
		void Create(LogicalDevice* pLogicalDevice, 
			const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets);
		void Destroy() const;

		std::vector<DescriptorSet> Allocate(DescriptorSetLayout& layout, uint32_t count) const;
	private:
		LogicalDevice* m_pLogicalDevice = nullptr;
		VkDescriptorPool m_pool = VK_NULL_HANDLE;
		uint32_t m_maxSets{};
	};
}