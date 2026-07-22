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
		PipelineLayout const * layout = nullptr;

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

		//Dynamic Rendering
		GraphicsPipelineBuilder& UseDynamicRendering(bool enableDynamic);
		GraphicsPipelineBuilder& AddColorAttachment(VkFormat format);
		GraphicsPipelineBuilder& SetDepthAttachment(VkFormat format);

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
		std::vector<VkVertexInputBindingDescription> m_bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;

		//Dynamic Rendering
		bool m_useDynamicRendering{ false };
		std::vector<VkFormat> m_colorAttachments{};
		VkFormat m_depthFormat{};
	};
}