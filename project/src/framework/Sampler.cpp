#include "Sampler.h"

#include "Common.h"
#include "Context.h"

void dae::Sampler::Destroy(const Context& context)
{
	vkDestroySampler(context.logicalDevice.device, sampler, nullptr);
}

void dae::SamplerBuilder::Build(const Context& context, Sampler& sampler) const
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.minFilter = m_filter;
	samplerInfo.magFilter = m_filter;

	samplerInfo.addressModeU = m_addressMode;
	samplerInfo.addressModeV = m_addressMode;
	samplerInfo.addressModeW = m_addressMode;

	if (context.physicalDevice.features.samplerAnisotropy)
	{
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = context.physicalDevice.properties.limits.maxSamplerAnisotropy;
	}
	else
	{
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1.f;
	}

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = m_enableCompare ? VK_TRUE : VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = m_mipmapMode;
	samplerInfo.mipLodBias = m_mipLodBias;
	samplerInfo.minLod = m_minLod;
	samplerInfo.maxLod = m_maxLod;

	LOG_VULKAN_ERROR(
		vkCreateSampler(context.logicalDevice.device, &samplerInfo, nullptr, &sampler.sampler),
		"failed to create sampler!"
	);
}

dae::SamplerBuilder& dae::SamplerBuilder::SetFilter(VkFilter filter)
{
	m_filter = filter;
	return *this;
}

dae::SamplerBuilder& dae::SamplerBuilder::SetAddressMode(VkSamplerAddressMode addressMode)
{
	m_addressMode = addressMode;
	return *this;
}

dae::SamplerBuilder& dae::SamplerBuilder::EnableAnisotropy(bool enableAnisotropy)
{
	m_enableAnisotropy = enableAnisotropy;
	return *this;
}

dae::SamplerBuilder& dae::SamplerBuilder::SetMipmapMode(VkSamplerMipmapMode mipmapMode)
{
	m_mipmapMode = mipmapMode;
	return *this;
}

dae::SamplerBuilder& dae::SamplerBuilder::EnableCompare(bool enableCompare)
{
	m_enableCompare = enableCompare;
	return *this;
}

dae::SamplerBuilder& dae::SamplerBuilder::SetCompareOp(VkCompareOp compareOp)
{
	m_compareOp = compareOp;
	return *this;
}

dae::SamplerBuilder& dae::SamplerBuilder::SetMipLodBias(float mipLodBias)
{
	m_mipLodBias = mipLodBias;
	return *this;
}

dae::SamplerBuilder& dae::SamplerBuilder::SetMinLod(float minLod)
{
	m_minLod = minLod;
	return *this;
}

dae::SamplerBuilder& dae::SamplerBuilder::SetMaxLod(float maxLod)
{
	m_maxLod = maxLod;
	return *this;
}
