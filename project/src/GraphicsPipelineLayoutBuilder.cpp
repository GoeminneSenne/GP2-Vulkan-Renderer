#include "GraphicsPipelineLayoutBuilder.h"

#include "Common.h"
#include "LogicalDevice.h"
#include "DescriptorSetLayout.h"
#include "GraphicsPipelineLayout.h"

void dae::GraphicsPipelineLayoutBuilder::Build(const LogicalDevice& logicalDevice,
                                               GraphicsPipelineLayout& pipelineLayout)
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(m_setLayouts.size());
    pipelineLayoutInfo.pSetLayouts = m_setLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    LOG_VULKAN_ERROR(
        vkCreatePipelineLayout(logicalDevice.device, &pipelineLayoutInfo, nullptr, &pipelineLayout.layout),
		"failed to create pipeline layout!"
    );
}

dae::GraphicsPipelineLayoutBuilder& dae::GraphicsPipelineLayoutBuilder::AddDescriptorSetLayout(
	DescriptorSetLayout setLayout)
{
	m_setLayouts.push_back(setLayout.layout);
    return *this;
}
