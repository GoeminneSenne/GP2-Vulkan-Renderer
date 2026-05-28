#pragma once

#include <vulkan/vulkan_core.h>
#include "Context.h"

namespace dae
{
	struct RenderPass final
	{
		VkRenderPass handle = VK_NULL_HANDLE;

		void Destroy(const Context& context) const;
	};

	class RenderPassBuilder final
	{
	public:
		void Build(const Context& context, RenderPass& renderPass) const;

		RenderPassBuilder& AddColorAttachment(VkFormat format);
		RenderPassBuilder& SetDepthAttachment(VkFormat format);
		RenderPassBuilder& SetSamples(VkSampleCountFlagBits samples);

	private:
		std::vector<VkAttachmentDescription> m_attachments;
		std::vector<VkAttachmentReference> m_colorRefs;
		VkAttachmentReference m_depthRef{};
		bool m_hasDepthAttachment{ false };
		VkSampleCountFlagBits m_samples = VK_SAMPLE_COUNT_1_BIT;

	};
}