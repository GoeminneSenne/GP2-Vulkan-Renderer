#pragma once

#include <vulkan/vulkan_core.h>


namespace dae
{
	struct Context;

	struct Sampler final
	{
		VkSampler handle = VK_NULL_HANDLE;

		void Destroy(const Context& context);
	};


	class SamplerBuilder final
	{
	public:
		void Build(const Context& context, Sampler& sampler) const;

		SamplerBuilder& SetFilter(VkFilter filter);
		SamplerBuilder& SetAddressMode(VkSamplerAddressMode addressMode);
		SamplerBuilder& EnableAnisotropy(bool enableAnisotropy);
		SamplerBuilder& SetMipmapMode(VkSamplerMipmapMode mipmapMode);
		SamplerBuilder& EnableCompare(bool enableCompare);
		SamplerBuilder& SetCompareOp(VkCompareOp compareOp);
		SamplerBuilder& SetMipLodBias(float mipLodBias);
		SamplerBuilder& SetMinLod(float minLod);
		SamplerBuilder& SetMaxLod(float maxLod);

	private:
		VkFilter m_filter = VK_FILTER_LINEAR;
		VkSamplerAddressMode m_addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		bool m_enableAnisotropy = false;
		VkSamplerMipmapMode m_mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		bool m_enableCompare{ false };
		VkCompareOp m_compareOp = VK_COMPARE_OP_ALWAYS;
		float m_mipLodBias = 0.f;
		float m_minLod = 0.f;
		float m_maxLod = VK_LOD_CLAMP_NONE;

	};
}