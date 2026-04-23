#include "DescriptorSetLayoutBuilder.h"

#include "Common.h"
#include "LogicalDevice.h"

void dae::DescriptorSetLayoutBuilder::Build(const LogicalDevice& logicalDevice, DescriptorSetLayout& descriptorSetLayout)
{
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

	for (const auto& binding : m_bindings )
	{
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding.binding;
		layoutBinding.descriptorType = binding.type;
		layoutBinding.stageFlags = binding.stages;
		layoutBinding.descriptorCount = 1;
		layoutBinding.pImmutableSamplers = nullptr;

		layoutBindings.push_back(layoutBinding);
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
	layoutInfo.pBindings = layoutBindings.data();

	LOG_VULKAN_ERROR(
		vkCreateDescriptorSetLayout(logicalDevice.device, &layoutInfo, nullptr, &descriptorSetLayout.layout),
		"failed to create descriptor set layout!"
	);

	descriptorSetLayout.bindings = std::move(m_bindings);
}

dae::DescriptorSetLayoutBuilder& dae::DescriptorSetLayoutBuilder::AddBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stages, uint32_t count)
{
	m_bindings.emplace_back(binding, type, stages, count);
	return *this;
}
