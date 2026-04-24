#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace dae
{
	struct Context;
	struct Shader;
	struct GraphicsPipeline;
	struct RenderPass;
	struct GraphicsPipelineLayout;
	struct LogicalDevice;

	class GraphicsPipelineBuilder final
	{
	public:

		void Build(const Context& context, GraphicsPipeline& pipeline);

		GraphicsPipelineBuilder& SetPipelineLayout(const GraphicsPipelineLayout& layout);
		GraphicsPipelineBuilder& SetRenderPass(const RenderPass& renderPass);
		GraphicsPipelineBuilder& SetShader(const Shader& shader, VkShaderStageFlagBits stageBits);
		GraphicsPipelineBuilder& SetInputTopology(VkPrimitiveTopology primitiveTopology);
		GraphicsPipelineBuilder& SetCullMode(VkCullModeFlagBits cullMode, VkFrontFace frontFace);
		GraphicsPipelineBuilder& SetPolygonMode(VkPolygonMode polygonMode);
		GraphicsPipelineBuilder& SetDepthTest(bool enableTest, bool enableWrite, VkCompareOp compareOp);
		GraphicsPipelineBuilder& AddVertexInputBindingDescription(VkVertexInputBindingDescription bindingDescription);
		GraphicsPipelineBuilder& AddVertexInputAttributeDescriptions(const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);

	private:
		const GraphicsPipelineLayout* m_layout = nullptr;
		const RenderPass* m_renderPass = nullptr;

		std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
		VkPrimitiveTopology m_primitiveTopology{};
		VkCullModeFlagBits m_cullMode{};
		VkFrontFace m_frontFace{};
		VkPolygonMode m_polygonMode{};
		bool m_depthTestEnable{ false };
		bool m_depthWriteEnable{ false };
		VkCompareOp m_depthCompareOp{};
		//Multisampling
		std::vector<VkVertexInputBindingDescription> m_bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;
	};
}