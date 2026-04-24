#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace dae
{
	struct Context;
	struct DescriptorSet;
	struct DescriptorSetLayout;
	class LogicalDevice;

	class DescriptorPool final
	{
	public:
		void Create(const Context& context, 
			const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets);
		void Destroy(const Context& context) const;

		std::vector<DescriptorSet> Allocate(const Context& context, DescriptorSetLayout& layout, uint32_t count) const;
	private:
		VkDescriptorPool m_pool = VK_NULL_HANDLE;
		uint32_t m_maxSets{};
	};
}