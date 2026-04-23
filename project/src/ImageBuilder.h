#pragma once

#include <vulkan/vulkan_core.h>

#include "vk_mem_alloc.h"

namespace dae
{
	struct Image;
	struct LogicalDevice;

	class ImageBuilder final
	{
	public:

		void Build(const VmaAllocator& allocator, const LogicalDevice& logicalDevice, Image& image);

		ImageBuilder& SetWidth(uint32_t width);
		ImageBuilder& SetHeight(uint32_t height);
		ImageBuilder& SetMipLevels(uint32_t mipLevels);
		ImageBuilder& SetFormat(VkFormat format);
		ImageBuilder& SetTiling(VkImageTiling tiling);
		ImageBuilder& SetUsageFlags(VkImageUsageFlags usageFlags);
		ImageBuilder& SetAspectMask(VkImageAspectFlags aspectMask);
		ImageBuilder& SetAllocUsage(VmaMemoryUsage allocUsage);
		ImageBuilder& SetAllocFlags(VmaAllocationCreateFlags allocFlags);

	private:
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		uint32_t m_mipLevels = 1;
		VkFormat m_format = VK_FORMAT_UNDEFINED;
		VkImageTiling m_tiling = VK_IMAGE_TILING_OPTIMAL;
		VkImageUsageFlags m_usageFlags = 0;
		VkImageAspectFlags m_aspectMask = 0;
		VmaMemoryUsage m_allocUsage = VMA_MEMORY_USAGE_AUTO;
		VmaAllocationCreateFlags m_allocFlags = 0;
	};
}
