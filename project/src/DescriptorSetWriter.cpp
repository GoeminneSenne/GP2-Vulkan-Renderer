#include "DescriptorSetWriter.h"

#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"
#include "LogicalDevice.h"
#include "Buffer.h"
#include "Image.h"
#include "Sampler.h"

dae::DescriptorSetWriter& dae::DescriptorSetWriter::SetTarget(DescriptorSet* pTarget)
{
	m_target = pTarget;
	return *this;
}

dae::DescriptorSetWriter& dae::DescriptorSetWriter::WriteBuffer(uint32_t binding, const Buffer& buffer, VkDeviceSize offset,
	VkDeviceSize range)
{
	m_bufferInfo = {};
	m_bufferInfo.buffer = buffer.buffer;
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
	m_imageInfo = {};
	m_imageInfo.imageLayout = layout;
	m_imageInfo.imageView = image.view;
	m_imageInfo.sampler = sampler.sampler;

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = m_target->set;
	write.dstBinding = binding;
	write.dstArrayElement = 0;
	write.descriptorType = m_target->pLayout->bindings[binding].type;
	write.descriptorCount = 1;
	write.pImageInfo = &m_imageInfo;

	m_writes.push_back(write);
	return *this;
}

void dae::DescriptorSetWriter::UpdateTarget(const Context& context) const
{
	vkUpdateDescriptorSets(context.logicalDevice.device, static_cast<uint32_t>(m_writes.size()), m_writes.data(), 0, nullptr);
}
