#include "VulkanDemoApplication.h"

#include <array>
#include <chrono>

#include "../framework/DescriptorSet.h"
#include "../framework/Sampler.h"
#include "../framework/RenderPass.h"
#include "../framework/Shader.h"
#include "../framework/SwapchainHelpers.h"
#include "../framework/Vertex.h"
#include "../framework/Framebuffer.h"
#include "../framework/ImageHelpers.h"


void dae::VulkanDemoApplication::Init()
{
    // --- Window -----------------------------------
	{
        m_window.InitWindow(WIDTH, HEIGHT, "Vulkan Demo");
        m_applicationDeletionQueue.PushDeletor([&]() {m_window.Destroy(); });
	}
    // --- Context ----------------------------------
	{
        ContextConfig config{
			ENABLE_VALIDATION,
			validationLayers,
			{},
			deviceExtensions
        };

        ContextFactory contextFactory{ config };
        contextFactory.Create(&m_window, m_context);
        m_applicationDeletionQueue.PushDeletor([&]() {m_context.Destroy(); });
	}
    // --- Swapchain ----------------------------------
    {
        m_swapchain.Create(m_window, m_context);
        m_applicationDeletionQueue.PushDeletor([&]() {m_swapchain.Destroy(m_context); });
    }
    // --- RenderPass -------------------------------
	{
        RenderPassBuilder renderPassBuilder = {};
        renderPassBuilder
            .AddColorAttachment(m_swapchain.imageFormat)
            .SetDepthAttachment(FindDepthFormat())
            .Build(m_context, m_renderPass);

        m_applicationDeletionQueue.PushDeletor([&]() {m_renderPass.Destroy(m_context); });
	}
    // --- DescriptorSetLayout ----------------------
	{
        DescriptorSetLayoutBuilder layoutBuilder{};
        layoutBuilder
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
            .AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .Build(m_context, m_descriptorSetLayout);

        m_applicationDeletionQueue.PushDeletor([&]() {m_descriptorSetLayout.Destroy(m_context); });

	}
	// --- Graphics Pipeline ------------------------
	{
        Shader vertexShader, fragmentShader;

        ShaderLoader shaderLoader = {};
        shaderLoader
            .SetFilename("resources/shaders/base_shader.vert.spv")
            .Load(m_context, vertexShader);

        shaderLoader
            .SetFilename("resources/shaders/base_shader.frag.spv")
            .Load(m_context, fragmentShader);

        GraphicsPipelineLayoutBuilder pipelineLayoutBuilder = {};
        pipelineLayoutBuilder
            .AddDescriptorSetLayout(m_descriptorSetLayout)
            .Build(m_context, m_graphicsPipelineLayout);

        m_applicationDeletionQueue.PushDeletor([&]() {m_graphicsPipelineLayout.Destroy(m_context); });


        GraphicsPipelineBuilder pipelineBuilder = {};
        pipelineBuilder
            .SetPipelineLayout(m_graphicsPipelineLayout)
            .SetRenderPass(m_renderPass)
            .SetShader(vertexShader, VK_SHADER_STAGE_VERTEX_BIT)
            .SetShader(fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT)
            .SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .SetCullMode(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE)
            .SetPolygonMode(VK_POLYGON_MODE_FILL)
            .SetDepthTest(true, true, VK_COMPARE_OP_LESS)
            .AddVertexInputBindingDescription(Vertex::GetBindingDescription())
            .AddVertexInputAttributeDescriptions(Vertex::GetAttributeDescriptions())
            .Build(m_context, m_graphicsPipeline);

        m_applicationDeletionQueue.PushDeletor([&]() {m_graphicsPipeline.Destroy(m_context); });


        fragmentShader.Destroy(m_context);
        vertexShader.Destroy(m_context);

	}
	// --- Depth Resources --------------------------
	{
		CreateDepthResources();
		
	}
	// --- Framebuffers ------------------------------
	{
        SwapchainHelpers::CreateFramebuffers(m_context, m_renderPass, m_depthImage, m_swapchain);
	}
	// --- Texture Sampler --------------------------
	{
        SamplerBuilder samplerBuilder = {};
        samplerBuilder
            .SetFilter(VK_FILTER_LINEAR)
            .SetAddressMode(VK_SAMPLER_ADDRESS_MODE_REPEAT)
            .EnableAnisotropy(true)
            .EnableCompare(false)
            .SetMipmapMode(VK_SAMPLER_MIPMAP_MODE_LINEAR)
            .SetMipLodBias(0.f)
            .SetMinLod(0.f)
            .SetMaxLod(VK_LOD_CLAMP_NONE)
            .Build(m_context, m_sampler);

        m_applicationDeletionQueue.PushDeletor([&]() {m_sampler.Destroy(m_context); });
	}

	// --- Scene Data -------------------------------
	{
        ImageHelpers::CreateTexture(m_context, TEXTURE_PATH, m_textureImage);
        m_applicationDeletionQueue.PushDeletor([&]() {m_textureImage.Destroy(m_context); });

        m_pMesh = std::make_unique<Mesh>();
        m_pMesh->LoadObj(m_context, MODEL_PATH);
        m_applicationDeletionQueue.PushDeletor([&]() {m_pMesh->Destroy(m_context); });
		
	}
	// --- Uniform Buffers --------------------------
	{
        m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            VkDeviceSize bufferSize = sizeof(UniformBufferObject);
            m_uniformBuffers[i].CreateUniform(m_context, bufferSize);

            vmaMapMemory(m_context.allocator, m_uniformBuffers[i].allocation, &uniformBuffersMapped[i]);
        }
	}
	// --- Descriptor Pool --------------------------
	{
        std::vector<VkDescriptorPoolSize> poolSizes;
        poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));
        poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));

        m_descriptorPool.Create(m_context, poolSizes, MAX_FRAMES_IN_FLIGHT);
        m_applicationDeletionQueue.PushDeletor([&]() {m_descriptorPool.Destroy(m_context); });
	}

	// --- Descriptor Sets --------------------------
	{
        m_descriptorSets = m_descriptorPool.Allocate(m_context, m_descriptorSetLayout, MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            DescriptorSetWriter writer = {};
            writer
                .SetTarget(&m_descriptorSets[i])
                .WriteBuffer(0, m_uniformBuffers[i], 0, sizeof(UniformBufferObject))
                .WriteImage(1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_textureImage, m_sampler)
                .UpdateTarget(m_context);
        }
	}

	// --- Sync Manager -----------------------------
	{
	    m_syncManager.Create(m_context, m_swapchain.images.size());
        m_applicationDeletionQueue.PushDeletor([&]() {m_syncManager.Destroy(m_context); });
	}
	    
	// --- Camera -----------------------------
	{
        m_camera.Initialize(90.f, { 0.f, -2.f, 0.f }, 0.1f, 10.f);
        m_camera.CalculateProjectionMatrix(m_swapchain.extent.width, m_swapchain.extent.height);
	}

	m_renderCommandBuffers = m_context.commandPool.Allocate(MAX_FRAMES_IN_FLIGHT);
}

bool dae::VulkanDemoApplication::ShouldClose() const
{
	return m_window.ShouldClose();
}

void dae::VulkanDemoApplication::PollEvents() const
{
	m_window.PollEvents();
}

void dae::VulkanDemoApplication::Update()
{
    m_timer.Update();
    m_camera.Update(m_timer, m_window);

    UpdateUniformBuffer(m_syncManager.GetCurrentFrameIndex()); 
}

void dae::VulkanDemoApplication::DrawFrame()
{
    uint32_t currentFrame = m_syncManager.GetCurrentFrameIndex();

    // --- Wait for + acquire fence --------------------------
    const Fence* inFlightFence = m_syncManager.GetInFlightFence(currentFrame);
    const Semaphore* imageAvailableSemaphore = m_syncManager.GetImageAvailableSemaphore(currentFrame);

    inFlightFence->Wait(m_context);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(m_context.logicalDevice.device, m_swapchain.swapchain, UINT64_MAX, imageAvailableSemaphore->GetHandle(), VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapchain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("failed to acquire swap chain image!");

    inFlightFence->Reset(m_context);

    ///Record
	// --- Record --------------------------
    vkResetCommandBuffer(m_renderCommandBuffers[currentFrame].commandBuffer, 0);
    RecordRenderCommandBuffer(m_renderCommandBuffers[currentFrame], imageIndex);

    // --- Submit --------------------------
	VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore->GetHandle() };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_renderCommandBuffers[currentFrame].commandBuffer;

    //Get semaphore using imageIndex instead of currentFrame!
    const Semaphore* renderFinishedSemaphore = m_syncManager.GetRenderFinishedSemaphore(imageIndex);

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore->GetHandle() };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    m_context.logicalDevice.graphicsQueue.Submit(submitInfo, *inFlightFence);

    // --- Present --------------------------
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { m_swapchain.swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = m_context.logicalDevice.presentQueue.Present(presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window.wasResized)
    {
        m_window.wasResized = false;
        RecreateSwapchain();
    }
    else if (result != VK_SUCCESS)
        throw std::runtime_error("failed to present swap chain images!");
	
    m_syncManager.NextFrame();
}

void dae::VulkanDemoApplication::Cleanup()
{
	vkDeviceWaitIdle(m_context.logicalDevice.device);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        vmaUnmapMemory(m_context.allocator, m_uniformBuffers[i].allocation);
        m_uniformBuffers[i].Destroy(m_context);
    }

    m_applicationDeletionQueue.Flush();
}


void dae::VulkanDemoApplication::CreateDepthResources()
{
    VkFormat depthFormat = FindDepthFormat();

    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    if (HasStencilComponent(depthFormat))
    {
        aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    ImageBuilder imageBuilder = {};
    imageBuilder
        .SetWidth(m_swapchain.extent.width)
        .SetHeight(m_swapchain.extent.height)
        .SetMipLevels(1)
        .SetFormat(depthFormat)
        .SetTiling(VK_IMAGE_TILING_OPTIMAL)
        .SetUsageFlags(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        .SetAspectMask(aspectMask)
        .SetAllocUsage(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE)
        .SetAllocFlags(0)
        .Build(m_context, m_depthImage);

    m_applicationDeletionQueue.PushDeletor([&]() {m_depthImage.Destroy(m_context); });


    CommandBuffer commandBuffer = m_context.commandPool.BeginSingleTimeCommand();
    ImageHelpers::TransitionImageLayout(commandBuffer, m_depthImage,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    m_context.commandPool.EndSingleTimeCommand(commandBuffer);
}

void dae::VulkanDemoApplication::CleanupSwapchain()
{
    m_depthImage.Destroy(m_context);
    m_swapchain.Destroy(m_context);
}

void dae::VulkanDemoApplication::RecreateSwapchain()
{
    int width = 0, height = 0;
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(m_window.window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_context.logicalDevice.device);

    CleanupSwapchain();

    m_swapchain.Create(m_window, m_context);
    CreateDepthResources();
    SwapchainHelpers::CreateFramebuffers(m_context, m_renderPass, m_depthImage, m_swapchain);

	//Todo add RenderFinishedSemaphores reconstruction in SyncManager
}

void dae::VulkanDemoApplication::RecordRenderCommandBuffer(CommandBuffer& commandBuffer, uint32_t imageIndex)
{
    commandBuffer.Begin();

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass.renderPass;
    renderPassInfo.framebuffer = m_swapchain.framebuffers[imageIndex].framebuffer;
    renderPassInfo.renderArea.offset = { 0,0 };
    renderPassInfo.renderArea.extent = m_swapchain.extent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer.commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline.pipeline);

    VkViewport viewport{};
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = static_cast<float>(m_swapchain.extent.width);
    viewport.height = static_cast<float>(m_swapchain.extent.height);
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;
    vkCmdSetViewport(commandBuffer.commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0,0 };
    scissor.extent = m_swapchain.extent;
    vkCmdSetScissor(commandBuffer.commandBuffer, 0, 1, &scissor);
    
    uint32_t currentFrame = m_syncManager.GetCurrentFrameIndex();
	vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipelineLayout.layout, 0, 1, &m_descriptorSets[currentFrame].set, 0, nullptr);

    m_pMesh->RecordDraw(commandBuffer);

    vkCmdEndRenderPass(commandBuffer.commandBuffer);

    commandBuffer.End();
}

VkFormat dae::VulkanDemoApplication::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
	VkFormatFeatureFlags features) const
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(m_context.physicalDevice.physicalDevice, format, &properties);

        if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

VkFormat dae::VulkanDemoApplication::FindDepthFormat() const
{
    return FindSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

bool dae::VulkanDemoApplication::HasStencilComponent(VkFormat format) const
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void dae::VulkanDemoApplication::UpdateUniformBuffer(uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
    ubo.view = m_camera.CalculateViewMatrix();
    ubo.proj = m_camera.projectionMatrix;

    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}
