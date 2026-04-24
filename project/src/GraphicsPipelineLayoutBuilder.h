#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace dae
{
	struct Context;
	struct GraphicsPipelineLayout;
	struct DescriptorSetLayout;
	struct LogicalDevice;

	class GraphicsPipelineLayoutBuilder final
	{
	public:
		void Build(const Context& context, GraphicsPipelineLayout& pipelineLayout);

		GraphicsPipelineLayoutBuilder& AddDescriptorSetLayout(DescriptorSetLayout setLayout);
		//GraphicsPipelineLayoutBuilder& AddPushConstant(VkPushConstantRange pushConstant);

	private:
		std::vector<VkDescriptorSetLayout> m_setLayouts;
		std::vector<VkPushConstantRange> m_pushConstants; //TODO
	};
}