#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace dae
{
	struct Context;
	struct Buffer;
	struct Image;
	struct Sampler;

	struct Binding final
	{
		uint32_t binding{};
		VkDescriptorType type{};
		VkShaderStageFlags stages{};
		uint32_t count{};
	};

	struct DescriptorSetLayout final
	{
		VkDescriptorSetLayout layout = VK_NULL_HANDLE;
		std::vector<Binding> bindings;

		void Destroy(const Context& context) const;
	};

	struct DescriptorSet final
	{
		VkDescriptorSet set = VK_NULL_HANDLE;
		DescriptorSetLayout* pLayout = nullptr;
	};

	class DescriptorSetWriter final
	{
	public:
		DescriptorSetWriter& SetTarget(DescriptorSet* pTarget);
		DescriptorSetWriter& WriteBuffer(uint32_t binding, const Buffer& buffer, VkDeviceSize offset, VkDeviceSize range);
		DescriptorSetWriter& WriteImage(uint32_t binding, VkImageLayout layout, const Image& image, const Sampler& sampler);
		DescriptorSetWriter& WriteImageArray(uint32_t binding, VkImageLayout layout, const std::vector<Image>& images, const Sampler& sampler);

		void UpdateTarget(const Context& context) const;

	private:
		DescriptorSet* m_target = nullptr;
		VkDescriptorBufferInfo m_bufferInfo{};
		VkDescriptorImageInfo m_imageInfo{}; //Keeping for single Writes, should be removed in end result
		std::vector<VkDescriptorImageInfo> m_imageInfos{};
		std::vector<VkWriteDescriptorSet> m_writes;
	};

	class DescriptorSetLayoutBuilder final
	{
	public:
		void Build(const Context& context, DescriptorSetLayout& descriptorSetLayout);

		DescriptorSetLayoutBuilder& AddBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stages, uint32_t count = 1);
	private:
		std::vector<Binding> m_bindings;
	};

	class DescriptorPool final
	{
	public:
		void Create(const Context& context,
			const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets);
		void Destroy(const Context& context) const;

		DescriptorSet Allocate(const Context& context, DescriptorSetLayout& layout) const;
		std::vector<DescriptorSet> Allocate(const Context& context, DescriptorSetLayout& layout, uint32_t count) const;
	private:
		VkDescriptorPool m_pool = VK_NULL_HANDLE;
		uint32_t m_maxSets{};
	};
}