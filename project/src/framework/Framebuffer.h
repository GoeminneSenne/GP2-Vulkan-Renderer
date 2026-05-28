#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

#include "RenderPass.h"

namespace dae
{
	struct Context;

	struct Framebuffer final
	{
		VkFramebuffer handle = VK_NULL_HANDLE;
		VkExtent2D extent{};
		std::vector<VkImageView> attachments;

		void Destroy(const Context& context) const;
	};

	class FramebufferBuilder final
	{
	public:
		void Build(const Context& context, Framebuffer& framebuffer);

		FramebufferBuilder& SetRenderPass(RenderPass renderPass);
		FramebufferBuilder& SetAttachments(const std::vector<VkImageView>& attachments);
		FramebufferBuilder& SetWidth(uint32_t width);
		FramebufferBuilder& SetHeight(uint32_t height);


	private:
		RenderPass m_renderPass{};
		std::vector<VkImageView> m_attachments;
		uint32_t m_width{};
		uint32_t m_height{};
	};
}
