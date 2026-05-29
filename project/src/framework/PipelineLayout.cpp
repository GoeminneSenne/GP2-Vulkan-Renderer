#include "PipelineLayout.h"
#include "Common.h"
#include "Context.h"
#include "DescriptorSet.h"

void dae::PipelineLayout::Destroy(const Context& context) const
{
	if (handle != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(context.logicalDevice.handle, handle, nullptr);
	}
}

void dae::PipelineLayoutBuilder::Build(const Context& context,
	PipelineLayout& pipelineLayout)
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(m_setLayouts.size());
	pipelineLayoutInfo.pSetLayouts = m_setLayouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(m_pushConstants.size());
	pipelineLayoutInfo.pPushConstantRanges = m_pushConstants.data();

	LOG_VULKAN_ERROR(
		vkCreatePipelineLayout(context.logicalDevice.handle, &pipelineLayoutInfo, nullptr, &pipelineLayout.handle),
		"failed to create pipeline layout!"
	);
}

dae::PipelineLayoutBuilder& dae::PipelineLayoutBuilder::AddDescriptorSetLayout(
	DescriptorSetLayout setLayout)
{
	m_setLayouts.push_back(setLayout.layout);
	return *this;
}

dae::PipelineLayoutBuilder& dae::PipelineLayoutBuilder::AddPushConstant(uint32_t offset, uint32_t size,
	VkShaderStageFlags stageFlags)
{
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.offset = offset;
	pushConstantRange.size = size;
	pushConstantRange.stageFlags = stageFlags;

	m_pushConstants.push_back(pushConstantRange);
	return *this;
}
