#include "GraphicsPipeline.h"

#include "Common.h"
#include "DescriptorSet.h"
#include "PipelineLayout.h"
#include "RenderPass.h"
#include "Shader.h"

void dae::GraphicsPipeline::Destroy(const Context& context) const
{
	if (handle != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(context.logicalDevice.handle, handle, nullptr);
	}
}

void dae::GraphicsPipelineBuilder::Build(const Context& context, GraphicsPipeline& pipeline)
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(m_bindingDescriptions.size());
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = m_bindingDescriptions.data();
	vertexInputInfo.pVertexAttributeDescriptions = m_attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = m_primitiveTopology;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = m_polygonMode;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = m_cullMode;
	rasterizer.frontFace = m_frontFace;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = m_depthTestEnable ? VK_TRUE : VK_FALSE;
	depthStencil.depthWriteEnable = m_depthWriteEnable ? VK_TRUE : VK_FALSE;
	depthStencil.depthCompareOp = m_depthCompareOp;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;

	std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(m_colorAttachments.size());
	for (auto& attachment : colorBlendAttachments)
	{
		attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
			| VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		attachment.blendEnable = VK_FALSE;
	}

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = colorBlendAttachments.size();
	colorBlending.pAttachments = colorBlendAttachments.data();
	colorBlending.blendConstants[0] = 0.f;
	colorBlending.blendConstants[1] = 0.f;
	colorBlending.blendConstants[2] = 0.f;
	colorBlending.blendConstants[3] = 0.f;

	std::vector<VkDynamicState> dynamicStates
		= { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = static_cast<uint32_t>(m_shaderStages.size());
	pipelineInfo.pStages = m_shaderStages.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = m_layout->handle;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;
	pipelineInfo.pDepthStencilState = &depthStencil;

	if (m_useDynamicRendering)
	{
		VkPipelineRenderingCreateInfo renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		renderingInfo.colorAttachmentCount = static_cast<uint32_t>(m_colorAttachments.size());
		renderingInfo.pColorAttachmentFormats = m_colorAttachments.data();
		renderingInfo.depthAttachmentFormat = m_depthFormat;

		pipelineInfo.renderPass = VK_NULL_HANDLE;
		pipelineInfo.pNext = &renderingInfo;
	}
	else
	{
		pipelineInfo.renderPass = m_renderPass->handle;
	}

	LOG_VULKAN_ERROR(
		vkCreateGraphicsPipelines(context.logicalDevice.handle, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.handle),
		"failed to create graphics pipeline!"
	);

	pipeline.layout = m_layout;
}

dae::GraphicsPipelineBuilder& dae::GraphicsPipelineBuilder::SetPipelineLayout(const PipelineLayout& layout)
{
	m_layout = &layout;
	return *this;
}

dae::GraphicsPipelineBuilder& dae::GraphicsPipelineBuilder::SetRenderPass(const RenderPass& renderPass)
{
	m_renderPass = &renderPass;
	return *this;
}

dae::GraphicsPipelineBuilder& dae::GraphicsPipelineBuilder::SetShader(const Shader& shader, VkShaderStageFlagBits stageBits)
{
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = stageBits;
	vertShaderStageInfo.module = shader.module;
	vertShaderStageInfo.pName = shader.entryPoint.c_str();

	m_shaderStages.push_back(vertShaderStageInfo);
	return *this;
}

dae::GraphicsPipelineBuilder& dae::GraphicsPipelineBuilder::SetInputTopology(VkPrimitiveTopology primitiveTopology)
{
	m_primitiveTopology = primitiveTopology;
	return *this;
}

dae::GraphicsPipelineBuilder& dae::GraphicsPipelineBuilder::SetCullMode(VkCullModeFlagBits cullMode,
	VkFrontFace frontFace)
{
	m_cullMode = cullMode;
	m_frontFace = frontFace;
	return *this;
}

dae::GraphicsPipelineBuilder& dae::GraphicsPipelineBuilder::SetPolygonMode(VkPolygonMode polygonMode)
{
	m_polygonMode = polygonMode;
	return *this;
}

dae::GraphicsPipelineBuilder& dae::GraphicsPipelineBuilder::SetDepthTest(bool enableTest, bool enableWrite,
	VkCompareOp compareOp)
{
	m_depthTestEnable = enableTest;
	m_depthWriteEnable = enableWrite;
	m_depthCompareOp = compareOp;
	return *this;
}

dae::GraphicsPipelineBuilder& dae::GraphicsPipelineBuilder::AddVertexInputBindingDescription(
	VkVertexInputBindingDescription bindingDescription)
{
	m_bindingDescriptions.push_back(bindingDescription);
	return *this;
}

dae::GraphicsPipelineBuilder& dae::GraphicsPipelineBuilder::AddVertexInputAttributeDescriptions(
	const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
{
	m_attributeDescriptions.insert(m_attributeDescriptions.end(), attributeDescriptions.begin(), attributeDescriptions.end());
	return *this;
}

dae::GraphicsPipelineBuilder& dae::GraphicsPipelineBuilder::UseDynamicRendering(bool enableDynamic)
{
	m_useDynamicRendering = enableDynamic;
	return *this;
}

dae::GraphicsPipelineBuilder& dae::GraphicsPipelineBuilder::AddColorAttachment(VkFormat format)
{
	m_colorAttachments.push_back(format);
	return *this;
}

dae::GraphicsPipelineBuilder& dae::GraphicsPipelineBuilder::SetDepthAttachment(VkFormat format)
{
	m_depthFormat = format;
	return *this;
}
