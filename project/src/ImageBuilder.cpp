#include "ImageBuilder.h"

void dae::ImageBuilder::Build(Image& image)
{
	//Set extra info from Image struct
	//Create VkImage
	//Create ImageView
	//Generate mipmaps -> als nodig
}

dae::ImageBuilder& dae::ImageBuilder::SetWidth(uint32_t width)
{
	m_width = width;
	return *this;
}

dae::ImageBuilder& dae::ImageBuilder::SetHeight(uint32_t height)
{
	m_height = height;
	return *this;
}

dae::ImageBuilder& dae::ImageBuilder::SetMipLevels(uint32_t mipLevels)
{
	m_mipLevels = mipLevels;
	return *this;
}

dae::ImageBuilder& dae::ImageBuilder::SetFormat(VkFormat format)
{
	m_format = format;
	return *this;
}

dae::ImageBuilder& dae::ImageBuilder::SetTiling(VkImageTiling tiling)
{
	m_tiling = tiling;
	return *this;
}

dae::ImageBuilder& dae::ImageBuilder::SetUsageFlags(VkImageUsageFlags usageFlags)
{
	m_usageFlags = usageFlags;
	return *this;
}

dae::ImageBuilder& dae::ImageBuilder::SetAspectMask(VkImageAspectFlags aspectMask)
{
	m_aspectMask = aspectMask;
	return *this;
}

dae::ImageBuilder& dae::ImageBuilder::SetAllocUsage(VmaMemoryUsage allocUsage)
{
	m_allocUsage = allocUsage;
	return *this;
}

dae::ImageBuilder& dae::ImageBuilder::SetAllocFlags(VmaAllocationCreateFlags allocFlags)
{
	m_allocFlags = allocFlags;
	return *this;
}
