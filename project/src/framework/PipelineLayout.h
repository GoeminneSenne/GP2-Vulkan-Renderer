#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

#include "CommandBuffer.h"

namespace dae
{
	struct Context;
	struct DescriptorSetLayout;

	struct PipelineLayout final
	{
		VkPipelineLayout handle = VK_NULL_HANDLE;
		
		void Destroy(const Context& context) const;
	};

	class PipelineLayoutBuilder final
	{
	public:
		void Build(const Context& context, PipelineLayout& pipelineLayout);

		PipelineLayoutBuilder& AddDescriptorSetLayout(DescriptorSetLayout setLayout);
		PipelineLayoutBuilder& AddPushConstant(uint32_t offset, uint32_t size, VkShaderStageFlags stageFlags);

	private:
		std::vector<VkDescriptorSetLayout> m_setLayouts;
		std::vector<VkPushConstantRange> m_pushConstants; //TODO
	};
}
