#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace dae
{
	struct LogicalDevice;
	struct Framebuffer;

	class FramebufferBuilder final
	{
	public:
		void Build(const LogicalDevice& logicalDevice, Framebuffer& framebuffer);

		FramebufferBuilder& SetRenderPass(VkRenderPass renderPass);
		FramebufferBuilder& SetAttachments(const std::vector<VkImageView>& attachments);
		FramebufferBuilder& SetWidth(uint32_t width);
		FramebufferBuilder& SetHeight(uint32_t height);


	private:
		VkRenderPass m_renderPass = VK_NULL_HANDLE; //TODO should be replaced when wrapper is implemented
		std::vector<VkImageView> m_attachments;
		uint32_t m_width{};
		uint32_t m_height{};
	};
}