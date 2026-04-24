#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace dae
{
	struct RenderPass;
	struct LogicalDevice;

	class RenderPassBuilder final
	{
	public:
		void Build(const LogicalDevice& logicalDevice, RenderPass& renderPass) const;

		RenderPassBuilder& AddColorAttachment(VkFormat format);
		RenderPassBuilder& SetDepthAttachment(VkFormat format);
		RenderPassBuilder& SetSamples(VkSampleCountFlagBits samples);

	private:
		std::vector<VkAttachmentDescription> m_attachments;
		std::vector<VkAttachmentReference> m_colorRefs;
		VkAttachmentReference m_depthRef{};
		bool m_hasDepthAttachment{false};
		VkSampleCountFlagBits m_samples = VK_SAMPLE_COUNT_1_BIT;

	};
}