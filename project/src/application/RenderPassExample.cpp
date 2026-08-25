#include "RenderPassExample.h"

#include <array>
#include <chrono>

#include <framework/DescriptorSet.h>
#include <framework/Sampler.h>
#include <framework/RenderPass.h>
#include <framework/Shader.h>
#include <framework/SwapchainHelpers.h>
#include <framework/Vertex.h>
#include <framework/Framebuffer.h>
#include <framework/ImageHelpers.h>
#include <framework/GLTFLoader.h>
#include <framework/DepthHelpers.h>
#include <framework/Utils.h>


void dae::RenderPassExampleApplication::Init()
{
    // --- Window -----------------------------------
    {
        m_window.InitWindow(WIDTH, HEIGHT, "Renderpass Example");
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
    // --- Scene Data -------------------------------
    {
        GLTFLoader loader;
        m_scene = loader.LoadScene(m_context, "resources/sponza/Sponza.gltf");
        m_applicationDeletionQueue.PushDeletor([&]() {m_scene.Destroy(m_context); });

        m_textureImage = m_scene.textures[0];
    }

    // --- Swapchain ----------------------------------
    {
        m_swapchain.Create(m_window, m_context);
        m_resizeDeletionQueue.PushDeletor([&]() {m_swapchain.Destroy(m_context); });
    }
    // --- RenderPass -------------------------------
    {
        RenderPassBuilder renderPassBuilder = {};
        renderPassBuilder
            .AddColorAttachment(m_swapchain.imageFormat)
            .SetDepthAttachment(DepthHelpers::FindDepthFormat(m_context))
            .Build(m_context, m_renderPass);

        m_applicationDeletionQueue.PushDeletor([&]() {m_renderPass.Destroy(m_context); });
    }
    // --- DescriptorSetLayouts ----------------------
    
    DescriptorSetLayoutBuilder layoutBuilder{};
    
    //Frame data set
    layoutBuilder
        .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.Build(m_context, m_frameDataSetLayout);
    m_applicationDeletionQueue.PushDeletor([&]() {m_frameDataSetLayout.Destroy(m_context); });

    //Global data set
    layoutBuilder
        //Ideally this is split so you don't link the sampler for every image
		.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, m_scene.textures.size())
		.Build(m_context, m_globalDataSetLayout);
    m_applicationDeletionQueue.PushDeletor([&]() {m_globalDataSetLayout.Destroy(m_context); });

    // --- Shaders ----------------------------------
    Shader vertexShader, fragmentShader;
    {
        ShaderLoader shaderLoader = {};
        shaderLoader
            .SetFilename("resources/shaders/sponza.vert.spv")
            .Load(m_context, vertexShader);

        shaderLoader
            .SetFilename("resources/shaders/sponza.frag.spv")
            //.SetFilename("resources/shaders/base_shader.frag.spv")
            .Load(m_context, fragmentShader);
    }
    
    // --- Graphics Pipeline ------------------------
    {
        PipelineLayoutBuilder pipelineLayoutBuilder = {};
        pipelineLayoutBuilder
			.AddPushConstant(0, sizeof(ModelPC), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.AddDescriptorSetLayout(m_globalDataSetLayout)
			.AddDescriptorSetLayout(m_frameDataSetLayout)
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
	    DepthHelpers::CreateDepthImage(m_context, m_swapchain, m_depthImage);
        m_resizeDeletionQueue.PushDeletor([&]() {m_depthImage.Destroy(m_context);});
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

    // --- Uniform Buffers --------------------------
    {
        m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            VkDeviceSize bufferSize = sizeof(ViewProjectionUBO);
            BufferHelpers::CreateBuffer(m_context, m_uniformBuffers[i], bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO,
                VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);

            vmaMapMemory(m_context.allocator, m_uniformBuffers[i].allocation, &uniformBuffersMapped[i]);
        }
    }
    // --- Descriptor Pool --------------------------
    {
        std::vector<VkDescriptorPoolSize> poolSizes;
        poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));
        poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<uint32_t>(m_scene.textures.size()));

        m_descriptorPool.Create(m_context, poolSizes, MAX_FRAMES_IN_FLIGHT + 1);
        m_applicationDeletionQueue.PushDeletor([&]() {m_descriptorPool.Destroy(m_context); });
    }

    // --- Descriptor Sets --------------------------
    {
        //global descriptor set
        m_globalDescriptorSet = m_descriptorPool.Allocate(m_context, m_globalDataSetLayout);

        DescriptorSetWriter globalWriter{};
        globalWriter
            .SetTarget(&m_globalDescriptorSet)
            .WriteImageArray(0, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_scene.textures, m_sampler)
			.UpdateTarget(m_context);


        m_frameDescriptorSets = m_descriptorPool.Allocate(m_context, m_frameDataSetLayout, MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            DescriptorSetWriter writer = {};
            writer
                .SetTarget(&m_frameDescriptorSets[i])
                .WriteBuffer(0, m_uniformBuffers[i], 0, sizeof(ViewProjectionUBO))
                //.WriteImage(1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_textureImage, m_sampler)
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
        m_camera.Initialize(90.f, { 0.f, 0.f, 1.f }, 0.1f, 3000.f);
        m_camera.CalculateProjectionMatrix(m_swapchain.extent.width, m_swapchain.extent.height);
    }

    m_renderCommandBuffers = m_context.commandPool.Allocate(MAX_FRAMES_IN_FLIGHT);


}

bool dae::RenderPassExampleApplication::ShouldClose() const
{
    return m_window.ShouldClose();
}

void dae::RenderPassExampleApplication::PollEvents() const
{
    m_window.PollEvents();
}

void dae::RenderPassExampleApplication::Update()
{
    m_timer.Update();
    m_camera.Update(m_timer, m_window);

    UpdateViewProjectionUBO(m_syncManager.GetCurrentFrameIndex());
}

void dae::RenderPassExampleApplication::DrawFrame()
{
    uint32_t currentFrame = m_syncManager.GetCurrentFrameIndex();

    // --- Wait for + acquire fence --------------------------
    const Fence* inFlightFence = m_syncManager.GetInFlightFence(currentFrame);
    const Semaphore* imageAvailableSemaphore = m_syncManager.GetImageAvailableSemaphore(currentFrame);

    inFlightFence->Wait(m_context);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(m_context.logicalDevice.handle, m_swapchain.handle, UINT64_MAX, imageAvailableSemaphore->GetHandle(), VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        HandleResize();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("failed to acquire swap chain image!");

    inFlightFence->Reset(m_context);

    // --- Record --------------------------
    vkResetCommandBuffer(m_renderCommandBuffers[currentFrame].handle, 0);
    RecordRenderCommandBuffer(m_renderCommandBuffers[currentFrame], imageIndex);

    // --- Submit --------------------------
    VkSemaphoreSubmitInfo waitSemaphoreInfo{};
    waitSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    waitSemaphoreInfo.semaphore = imageAvailableSemaphore->GetHandle();
    waitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkCommandBufferSubmitInfo commandInfo{};
    commandInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    commandInfo.commandBuffer = m_renderCommandBuffers[currentFrame].handle;

    //Get semaphore using imageIndex instead of currentFrame!
    const Semaphore* renderFinishedSemaphore = m_syncManager.GetRenderFinishedSemaphore(imageIndex);

    VkSemaphoreSubmitInfo signalSemaphoreInfo{};
    signalSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signalSemaphoreInfo.semaphore = renderFinishedSemaphore->GetHandle();
    signalSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo2 submitInfo2{};
    submitInfo2.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submitInfo2.waitSemaphoreInfoCount = 1;
    submitInfo2.pWaitSemaphoreInfos = &waitSemaphoreInfo;
    submitInfo2.commandBufferInfoCount = 1;
    submitInfo2.pCommandBufferInfos = &commandInfo;
    submitInfo2.signalSemaphoreInfoCount = 1;
    submitInfo2.pSignalSemaphoreInfos = &signalSemaphoreInfo;

    LOG_VULKAN_ERROR(
        vkQueueSubmit2(m_context.logicalDevice.graphicsQueue.GetHandle(), 1, &submitInfo2, inFlightFence->GetHandle()),
        "failed to submit draw command buffer"
    );

    // --- Present --------------------------
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore->GetHandle() };

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { m_swapchain.handle };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = m_context.logicalDevice.presentQueue.Present(presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window.wasResized)
    {
        m_window.wasResized = false;
        HandleResize();
    }
    else if (result != VK_SUCCESS)
        throw std::runtime_error("failed to present swap chain images!");

    m_syncManager.NextFrame();
}

void dae::RenderPassExampleApplication::Cleanup()
{
    vkDeviceWaitIdle(m_context.logicalDevice.handle);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        vmaUnmapMemory(m_context.allocator, m_uniformBuffers[i].allocation);
        m_uniformBuffers[i].Destroy(m_context);
    }

    m_resizeDeletionQueue.Flush();
    m_applicationDeletionQueue.Flush();
}

void dae::RenderPassExampleApplication::HandleResize()
{
    int width = 0, height = 0;
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(m_window.window, &width, &height);
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(m_context.logicalDevice.handle);

    // Destroy old resources
    m_resizeDeletionQueue.Flush();

    //Query Swapchain support with new window
    m_context.physicalDevice.swapchainSupport = vkUtils::QuerySwapChainSupport(m_context.instance, m_context.physicalDevice.handle);

    //Recreate swapchain
    m_swapchain.Create(m_window, m_context);
    m_resizeDeletionQueue.PushDeletor([&]() {m_swapchain.Destroy(m_context);});

    //Recreate depth image
    DepthHelpers::CreateDepthImage(m_context, m_swapchain, m_depthImage);
    m_resizeDeletionQueue.PushDeletor([&]() {m_depthImage.Destroy(m_context);});

    //Recreate framebuffers
    SwapchainHelpers::CreateFramebuffers(m_context, m_renderPass, m_depthImage, m_swapchain);

    //Recreate RenderFinishedSemaphores
    m_syncManager.RecreateRenderFinishedSemaphores(m_context, m_swapchain.images.size());

    //Adjust camere aspect ratios
    m_camera.CalculateProjectionMatrix(m_swapchain.extent.width, m_swapchain.extent.height);
}


void dae::RenderPassExampleApplication::RecordRenderCommandBuffer(CommandBuffer& commandBuffer, uint32_t imageIndex)
{
    commandBuffer.Begin();

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass.handle;
    renderPassInfo.framebuffer = m_swapchain.framebuffers[imageIndex].handle;
    renderPassInfo.renderArea.offset = { 0,0 };
    renderPassInfo.renderArea.extent = m_swapchain.extent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer.handle, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    vkCmdBindPipeline(commandBuffer.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline.handle);

    VkViewport viewport{};
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = static_cast<float>(m_swapchain.extent.width);
    viewport.height = static_cast<float>(m_swapchain.extent.height);
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;
    vkCmdSetViewport(commandBuffer.handle, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0,0 };
    scissor.extent = m_swapchain.extent;
    vkCmdSetScissor(commandBuffer.handle, 0, 1, &scissor);

    vkCmdBindDescriptorSets(commandBuffer.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipelineLayout.handle, 0, 1, &m_globalDescriptorSet.set, 0, nullptr);

    uint32_t currentFrame = m_syncManager.GetCurrentFrameIndex();
    vkCmdBindDescriptorSets(commandBuffer.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipelineLayout.handle, 1, 1, &m_frameDescriptorSets[currentFrame].set, 0, nullptr);

    m_scene.RecordDraw(commandBuffer, m_graphicsPipeline);

    vkCmdEndRenderPass(commandBuffer.handle);

    
    commandBuffer.End();
}

void dae::RenderPassExampleApplication::UpdateViewProjectionUBO(uint32_t currentImage)
{
    ViewProjectionUBO ubo{
        m_camera.viewMatrix,
        m_camera.projectionMatrix
	};

    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}
