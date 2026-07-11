#include "Image.h"

#include "Common.h"
#include "LogicalDevice.h"
#include "Context.h"
#include "ImageHelpers.h"

// IMAGE
///////////////////////////////
void dae::Image::Destroy(const Context& context)
{
	if (image != VK_NULL_HANDLE && allocation != nullptr)
	{
		vmaDestroyImage(context.allocator, image, allocation);
	}

	if (view != VK_NULL_HANDLE)
	{
		vkDestroyImageView(context.logicalDevice.handle, view, nullptr);
	}
}

// IMAGE BUILDER
//////////////////////////////////
void dae::ImageBuilder::Build(const Context& context, Image& image) const
{

	//Set extra info from Image struct
	image.format = m_format;
	image.extent.width = m_width;
	image.extent.height = m_height;
	image.aspectMask = m_aspectMask;
	image.mipLevels = m_mipLevels;

	//Create VkImage
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = m_width;
	imageInfo.extent.height = m_height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = m_mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.format = m_format;
	imageInfo.tiling = m_tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = m_usageFlags;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = m_allocUsage;
	allocInfo.flags = m_allocFlags;

	LOG_VULKAN_ERROR(
		vmaCreateImage(context.allocator, &imageInfo, &allocInfo, &image.image, &image.allocation, &image.allocInfo),
		"failed to create image!"
	);

	//Create ImageView
	ImageHelpers::CreateImageView(context, image);
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

