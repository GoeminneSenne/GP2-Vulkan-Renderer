#pragma once
#include <vector>

#include "DescriptorSetLayout.h"

namespace dae
{
	struct LogicalDevice;

	class DescriptorSetLayoutBuilder final
	{
	public:
		void Build(const Context& context, DescriptorSetLayout& descriptorSetLayout);

		DescriptorSetLayoutBuilder& AddBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stages, uint32_t count = 1);
	private:
		std::vector<Binding> m_bindings;
	};
}
