#pragma once



namespace dae
{
	struct Image
	{
		VkImage image;
		VmaAllocation allocation;
		VkImageView imageView;
		VkImageLayout currentLayout;
		uint32_t mipLevels;
	};
}