#pragma once

#include <vulkan/vulkan_core.h>
#include "Context.h"

namespace dae
{
	struct DescriptorSetLayout;
	struct Shader;
	struct RenderPass;
	struct PipelineLayout;

	struct GraphicsPipeline final
	{
		VkPipeline handle = VK_NULL_HANDLE;

		void Destroy(const Context& context) const;
	};

	class GraphicsPipelineBuilder final
	{
	public:

		void Build(const Context& context, GraphicsPipeline& pipeline);

		GraphicsPipelineBuilder& SetPipelineLayout(const PipelineLayout& layout);
		GraphicsPipelineBuilder& SetRenderPass(const RenderPass& renderPass);
		GraphicsPipelineBuilder& SetShader(const Shader& shader, VkShaderStageFlagBits stageBits);
		GraphicsPipelineBuilder& SetInputTopology(VkPrimitiveTopology primitiveTopology);
		GraphicsPipelineBuilder& SetCullMode(VkCullModeFlagBits cullMode, VkFrontFace frontFace);
		GraphicsPipelineBuilder& SetPolygonMode(VkPolygonMode polygonMode);
		GraphicsPipelineBuilder& SetDepthTest(bool enableTest, bool enableWrite, VkCompareOp compareOp);
		GraphicsPipelineBuilder& AddVertexInputBindingDescription(VkVertexInputBindingDescription bindingDescription);
		GraphicsPipelineBuilder& AddVertexInputAttributeDescriptions(const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);

	private:
		const PipelineLayout* m_layout = nullptr;
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

	class GraphicsPipelineLayoutBuilder final
	{
	public:
		void Build(const Context& context, PipelineLayout& pipelineLayout);

		GraphicsPipelineLayoutBuilder& AddDescriptorSetLayout(DescriptorSetLayout setLayout);
		//GraphicsPipelineLayoutBuilder& AddPushConstant(VkPushConstantRange pushConstant);

	private:
		std::vector<VkDescriptorSetLayout> m_setLayouts;
		std::vector<VkPushConstantRange> m_pushConstants; //TODO
	};
}