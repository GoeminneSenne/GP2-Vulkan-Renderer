#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace dae
{
	struct Context;
	struct LogicalDevice;
	struct DescriptorSet;
	struct Buffer;
	struct Image;
	struct Sampler;

	class DescriptorSetWriter final
	{
	public:
		DescriptorSetWriter& SetTarget(DescriptorSet* pTarget);
		DescriptorSetWriter& WriteBuffer(uint32_t binding, const Buffer& buffer, VkDeviceSize offset, VkDeviceSize range);
		DescriptorSetWriter& WriteImage(uint32_t binding, VkImageLayout layout, const Image& image, const Sampler& sampler);
		
		void UpdateTarget(const Context& context) const;

	private:
		DescriptorSet* m_target = nullptr;
		VkDescriptorBufferInfo m_bufferInfo{};
		VkDescriptorImageInfo m_imageInfo{};
		std::vector<VkWriteDescriptorSet> m_writes;
	};
}