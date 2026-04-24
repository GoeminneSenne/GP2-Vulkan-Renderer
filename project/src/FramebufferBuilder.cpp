#include "FramebufferBuilder.h"

#include "Common.h"
#include "Framebuffer.h"
#include "LogicalDevice.h"

void dae::FramebufferBuilder::Build(const LogicalDevice& logicalDevice, Framebuffer& framebuffer)
{
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(m_attachments.size());
    framebufferInfo.pAttachments = m_attachments.data();
    framebufferInfo.width = m_width;
    framebufferInfo.height = m_height;
    framebufferInfo.layers = 1;

    LOG_VULKAN_ERROR(
        vkCreateFramebuffer(logicalDevice.device, &framebufferInfo, nullptr, &framebuffer.framebuffer),
		"failed to create framebuffer!"
    );

    framebuffer.extent = { m_width, m_height };
    framebuffer.attachments = std::move(m_attachments);
}

dae::FramebufferBuilder& dae::FramebufferBuilder::SetRenderPass(VkRenderPass renderPass)
{
	m_renderPass = renderPass;
	return *this;
}

dae::FramebufferBuilder& dae::FramebufferBuilder::SetAttachments(const std::vector<VkImageView>& attachments)
{
	m_attachments.insert(m_attachments.end(), attachments.begin(), attachments.end());
	return *this;
}

dae::FramebufferBuilder& dae::FramebufferBuilder::SetWidth(uint32_t width)
{
	m_width = width;
	return *this;
}

dae::FramebufferBuilder& dae::FramebufferBuilder::SetHeight(uint32_t height)
{
	m_height = height;
	return *this;
}
