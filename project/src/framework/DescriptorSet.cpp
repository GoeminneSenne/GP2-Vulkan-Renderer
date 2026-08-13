#include "DescriptorSet.h"

#include "Buffer.h"
#include "Common.h"
#include "Context.h"
#include "Image.h"
#include "Sampler.h"

void dae::DescriptorSetLayout::Destroy(const Context& context) const
{
	vkDestroyDescriptorSetLayout(context.logicalDevice.handle, layout, nullptr);
}

dae::DescriptorSetWriter& dae::DescriptorSetWriter::SetTarget(DescriptorSet* pTarget)
{
	m_target = pTarget;
	return *this;
}

dae::DescriptorSetWriter& dae::DescriptorSetWriter::WriteBuffer(uint32_t binding, const Buffer& buffer, VkDeviceSize offset,
	VkDeviceSize range)
{
	m_bufferInfo = {};
	m_bufferInfo.buffer = buffer.handle;
	m_bufferInfo.offset = offset;
	m_bufferInfo.range = range;

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = m_target->set;
	write.dstBinding = binding;
	write.dstArrayElement = 0;
	write.descriptorType = m_target->pLayout->bindings[binding].type;
	write.descriptorCount = 1;
	write.pBufferInfo = &m_bufferInfo;
	write.pTexelBufferView = nullptr;

	m_writes.push_back(write);
	return *this;
}

dae::DescriptorSetWriter& dae::DescriptorSetWriter::WriteImage(uint32_t binding, VkImageLayout layout,
	const Image& image, const Sampler& sampler)
{
	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = layout;
	imageInfo.imageView = image.view;
	imageInfo.sampler = sampler.handle;

	m_imageInfos.push_back(imageInfo);

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = m_target->set;
	write.dstBinding = binding;
	write.dstArrayElement = 0;
	write.descriptorType = m_target->pLayout->bindings[binding].type;
	write.descriptorCount = 1;
	write.pImageInfo = &m_imageInfos.back();

	m_writes.push_back(write);
	return *this;
}

dae::DescriptorSetWriter& dae::DescriptorSetWriter::WriteImageArray(uint32_t binding, VkImageLayout layout, const std::vector<Image>& images,
	const Sampler& sampler)
{
	m_imageArrayInfos.resize(images.size());

	for (size_t idx{}; idx < images.size(); ++idx)
	{
		VkDescriptorImageInfo imageInfo{};
		imageInfo.sampler = sampler.handle;
		imageInfo.imageView = images[idx].view;
		imageInfo.imageLayout = layout;
	
		m_imageArrayInfos[idx] = imageInfo;
	}

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = m_target->set;
	write.dstBinding = binding;
	write.dstArrayElement = 0;
	write.descriptorType = m_target->pLayout->bindings[binding].type;
	write.descriptorCount = (uint32_t)images.size();
	write.pImageInfo = m_imageArrayInfos.data();

	m_writes.push_back(write);
	return *this;
}

void dae::DescriptorSetWriter::UpdateTarget(const Context& context) const
{
	vkUpdateDescriptorSets(context.logicalDevice.handle, static_cast<uint32_t>(m_writes.size()), m_writes.data(), 0, nullptr);
}

void dae::DescriptorSetLayoutBuilder::Build(const Context& context, DescriptorSetLayout& descriptorSetLayout)
{
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

	for (const auto& binding : m_bindings)
	{
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding.binding;
		layoutBinding.descriptorType = binding.type;
		layoutBinding.stageFlags = binding.stages;
		layoutBinding.descriptorCount = binding.count;
		layoutBinding.pImmutableSamplers = nullptr;

		layoutBindings.push_back(layoutBinding);
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
	layoutInfo.pBindings = layoutBindings.data();

	LOG_VULKAN_ERROR(
		vkCreateDescriptorSetLayout(context.logicalDevice.handle, &layoutInfo, nullptr, &descriptorSetLayout.layout),
		"failed to create descriptor set layout!"
	);

	descriptorSetLayout.bindings = std::move(m_bindings);
}

dae::DescriptorSetLayoutBuilder& dae::DescriptorSetLayoutBuilder::AddBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stages, uint32_t count)
{
	m_bindings.emplace_back(binding, type, stages, count);
	return *this;
}

void dae::DescriptorPool::Create(const Context& context,
	const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets)
{
	m_maxSets = maxSets;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = m_maxSets;

	LOG_VULKAN_ERROR(
		vkCreateDescriptorPool(context.logicalDevice.handle, &poolInfo, nullptr, &m_pool),
		"failed to create descriptor pool!"
	);
}

void dae::DescriptorPool::Destroy(const Context& context) const
{
	if (m_pool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(context.logicalDevice.handle, m_pool, nullptr);
	}
}

dae::DescriptorSet dae::DescriptorPool::Allocate(const Context& context, DescriptorSetLayout& layout) const
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_pool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &layout.layout;

	DescriptorSet set{};
	vkAllocateDescriptorSets(context.logicalDevice.handle, &allocInfo, &set.set);
	set.pLayout = &layout;

	return set;
}

std::vector<dae::DescriptorSet> dae::DescriptorPool::Allocate(const Context& context, DescriptorSetLayout& layout, uint32_t count) const
{
	std::vector<VkDescriptorSetLayout> layouts(count, layout.layout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_pool;
	allocInfo.descriptorSetCount = count;
	allocInfo.pSetLayouts = layouts.data();

	std::vector<VkDescriptorSet> rawSets(count);
	LOG_VULKAN_ERROR(
		vkAllocateDescriptorSets(context.logicalDevice.handle, &allocInfo, rawSets.data()),
		"failed to allocate descriptor sets!"
	);

	std::vector<DescriptorSet> sets(count);
	for (uint32_t idx{}; idx < count; ++idx)
	{
		sets[idx].set = rawSets[idx];
		sets[idx].pLayout = &layout;
	}

	return sets;
}
