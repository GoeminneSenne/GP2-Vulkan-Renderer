#include "DeferredApplication.h"

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


void dae::DeferredApplication::Init()
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

    // --- DescriptorSetLayouts ----------------------
    {

    DescriptorSetLayoutBuilder layoutBuilder{};

        //Frame data set
        layoutBuilder
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
            .Build(m_context, m_frameDataSetLayout);
        m_applicationDeletionQueue.PushDeletor([&]() {m_frameDataSetLayout.Destroy(m_context); });

        //Global data set
        layoutBuilder = {};
        layoutBuilder
            //Ideally this is split so you don't link the sampler for every image
            .AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, m_scene.textures.size())
            .Build(m_context, m_globalDataSetLayout);
        m_applicationDeletionQueue.PushDeletor([&]() {m_globalDataSetLayout.Destroy(m_context); });
    
        //Deferred Pipeline set
        layoutBuilder = {};
        layoutBuilder
            .AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
			.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
			.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
            .AddBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
            .Build(m_context, m_deferredSetLayout);
        m_applicationDeletionQueue.PushDeletor([&]() {m_deferredSetLayout.Destroy(m_context); });
    
        //Deferrd Camera Data sets
        layoutBuilder = {};
        layoutBuilder
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .Build(m_context, m_deferredCameraSetLayout);
        m_applicationDeletionQueue.PushDeletor([&]() {m_deferredCameraSetLayout.Destroy(m_context); });
    }

    // --- Shaders ----------------------------------
    Shader vertexShader, fragmentShader;
    {
        ShaderLoader shaderLoader = {};
        shaderLoader
            .SetFilename("resources/shaders/geometry.vert.spv")
            .Load(m_context, vertexShader);

        shaderLoader
            .SetFilename("resources/shaders/geometry.frag.spv")
            .Load(m_context, fragmentShader);
    }
    // --- GBuffer Resources --------------------------
    {
        //Albedo Image
        ImageBuilder imageBuilder{};
        imageBuilder
            .SetWidth(m_swapchain.extent.width)
            .SetHeight(m_swapchain.extent.height)
            .SetMipLevels(1)
            .SetFormat(VK_FORMAT_R8G8B8A8_UNORM)
            .SetTiling(VK_IMAGE_TILING_OPTIMAL)
            .SetUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
            .SetAspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
            .SetAllocUsage(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE)
            .Build(m_context, m_gBuffer.albedoImage);

        //Normal Image
        imageBuilder = {};
        imageBuilder
            .SetWidth(m_swapchain.extent.width)
            .SetHeight(m_swapchain.extent.height)
            .SetMipLevels(1)
            .SetFormat(VK_FORMAT_R16G16B16A16_SFLOAT)
            .SetTiling(VK_IMAGE_TILING_OPTIMAL)
            .SetUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
            .SetAspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
            .SetAllocUsage(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE)
            .Build(m_context, m_gBuffer.normalImage);

        //Depth Image
        DepthHelpers::CreateDepthImage(m_context, m_swapchain, m_gBuffer.depthImage);

        // Metallic & Roughness Image
        imageBuilder = {};
        imageBuilder
            .SetWidth(m_swapchain.extent.width)
            .SetHeight(m_swapchain.extent.height)
            .SetMipLevels(1)
            .SetFormat(VK_FORMAT_R8G8B8A8_UNORM)
            .SetTiling(VK_IMAGE_TILING_OPTIMAL)
            .SetUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
            .SetAspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
            .SetAllocUsage(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE)
            .Build(m_context, m_gBuffer.metallicRoughnessImage);

        m_resizeDeletionQueue.PushDeletor([&]() {m_gBuffer.Destroy(m_context);});
    }

    // --- Geometry Pipeline ------------------------
    {
        PipelineLayoutBuilder pipelineLayoutBuilder = {};
        pipelineLayoutBuilder
            .AddPushConstant(0, sizeof(PushConstants), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
            .AddDescriptorSetLayout(m_globalDataSetLayout)
            .AddDescriptorSetLayout(m_frameDataSetLayout)
            .Build(m_context, m_geometryPipelineLayout);

        m_applicationDeletionQueue.PushDeletor([&]() {m_geometryPipelineLayout.Destroy(m_context); });

        GraphicsPipelineBuilder pipelineBuilder = {};
        pipelineBuilder
            .SetPipelineLayout(m_geometryPipelineLayout)
            .UseDynamicRendering(true)
            .AddColorAttachment(m_gBuffer.albedoImage.format)
            .AddColorAttachment(m_gBuffer.normalImage.format)
            .AddColorAttachment(m_gBuffer.metallicRoughnessImage.format)
            .SetDepthAttachment(DepthHelpers::FindDepthFormat(m_context))
            .SetShader(vertexShader, VK_SHADER_STAGE_VERTEX_BIT)
            .SetShader(fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT)
            .SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .SetCullMode(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE)
            .SetPolygonMode(VK_POLYGON_MODE_FILL)
            .SetDepthTest(true, false, VK_COMPARE_OP_LESS_OR_EQUAL)
            .AddVertexInputBindingDescription(Vertex::GetBindingDescription())
            .AddVertexInputAttributeDescriptions(Vertex::GetAttributeDescriptions())
            .Build(m_context, m_geometryPipeline);

        m_applicationDeletionQueue.PushDeletor([&]() {m_geometryPipeline.Destroy(m_context); });

    }

    // --- Depth Prepass Pipeline ------------------------
    {
        Shader prepassFragmentShader{};
        ShaderLoader loader{};
        loader
            .SetFilename("resources/shaders/prepass.frag.spv")
            .Load(m_context, prepassFragmentShader);

        GraphicsPipelineBuilder pipelineBuilder = {};
        pipelineBuilder
            .SetPipelineLayout(m_geometryPipelineLayout)
            .UseDynamicRendering(true)
            .SetDepthAttachment(DepthHelpers::FindDepthFormat(m_context))
            .SetShader(vertexShader, VK_SHADER_STAGE_VERTEX_BIT)
            .SetShader(prepassFragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT)
            .SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .SetCullMode(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE)
            .SetPolygonMode(VK_POLYGON_MODE_FILL)
            .SetDepthTest(true, true, VK_COMPARE_OP_LESS)
            .AddVertexInputBindingDescription((Vertex::GetBindingDescription()))
            .AddVertexInputAttributeDescriptions(Vertex::GetAttributeDescriptions())
            .Build(m_context, m_prepassPipeline);

        m_applicationDeletionQueue.PushDeletor([&]() {m_prepassPipeline.Destroy(m_context); });
        prepassFragmentShader.Destroy(m_context);
    }

    fragmentShader.Destroy(m_context);
    vertexShader.Destroy(m_context);

    // --- Deferred Graphics Pipeline ---------------
    {
        PipelineLayoutBuilder layoutBuilder = {};
        layoutBuilder
            .AddDescriptorSetLayout(m_deferredSetLayout)
            .AddDescriptorSetLayout(m_deferredCameraSetLayout)
            .Build(m_context, m_deferredPipelineLayout);
        m_applicationDeletionQueue.PushDeletor([&]() {m_deferredPipelineLayout.Destroy(m_context);});

        Shader deferredVertexShader, deferredFragmentShader;
        ShaderLoader loader = {};
        loader
            .SetFilename("resources/shaders/deferred_lighting.vert.spv")
            .Load(m_context, deferredVertexShader);
        loader
            .SetFilename("resources/shaders/deferred_lighting.frag.spv")
            .Load(m_context, deferredFragmentShader);
        

        GraphicsPipelineBuilder pipelineBuilder = {};
        pipelineBuilder
            .SetPipelineLayout(m_deferredPipelineLayout)
            .UseDynamicRendering(true)
            .AddColorAttachment(m_swapchain.imageFormat)
            .SetShader(deferredVertexShader, VK_SHADER_STAGE_VERTEX_BIT)
            .SetShader(deferredFragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT)
            .SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE)
            .SetPolygonMode(VK_POLYGON_MODE_FILL)
            .SetDepthTest(false, false, VK_COMPARE_OP_ALWAYS)
            .Build(m_context, m_deferredPipeline);

        m_applicationDeletionQueue.PushDeletor([&]() {m_deferredPipeline.Destroy(m_context);});
        deferredVertexShader.Destroy(m_context);
        deferredFragmentShader.Destroy(m_context);
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
        m_viewProjUBOs.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            VkDeviceSize bufferSize = sizeof(ViewProjectionUBO);
            BufferHelpers::CreateUniform(m_context, m_viewProjUBOs[i], bufferSize);
            m_applicationDeletionQueue.PushDeletor([&, i]() {m_viewProjUBOs[i].Destroy(m_context); });
        }

        m_deferredCameraDataUBOs.resize(MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            VkDeviceSize bufferSize = sizeof(CameraDataUBO);
            BufferHelpers::CreateUniform(m_context, m_deferredCameraDataUBOs[i], bufferSize);
            m_applicationDeletionQueue.PushDeletor([&, i]() {m_deferredCameraDataUBOs[i].Destroy(m_context);});
        }
    }
    // --- Descriptor Pool --------------------------
    {
        std::vector<VkDescriptorPoolSize> poolSizes;
        poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));
        poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));
        poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<uint32_t>(m_scene.textures.size()));
        poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4);

        m_descriptorPool.Create(m_context, poolSizes, 2 * MAX_FRAMES_IN_FLIGHT + 1 + 1);
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

        //frame descriptor set
        m_frameDescriptorSets = m_descriptorPool.Allocate(m_context, m_frameDataSetLayout, MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            DescriptorSetWriter writer = {};
            writer
                .SetTarget(&m_frameDescriptorSets[i])
                .WriteBuffer(0, m_viewProjUBOs[i], 0, sizeof(ViewProjectionUBO))
                .UpdateTarget(m_context);
        }

        //deferred descriptor set
        m_deferredDescriptorSet = m_descriptorPool.Allocate(m_context, m_deferredSetLayout);

        auto* pTarget = &m_deferredDescriptorSet;

        DescriptorSetWriter deferredWriter{};
        deferredWriter
            .SetTarget(&m_deferredDescriptorSet)
            .WriteImage(0, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_gBuffer.albedoImage, m_sampler)
			.WriteImage(1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_gBuffer.depthImage, m_sampler)
			.WriteImage(2, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_gBuffer.normalImage, m_sampler)
            .WriteImage(3, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_gBuffer.metallicRoughnessImage, m_sampler)
            .UpdateTarget(m_context);

        m_deferredCameraSets = m_descriptorPool.Allocate(m_context, m_deferredCameraSetLayout, MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            DescriptorSetWriter writer = {};
            writer
                .SetTarget(&m_deferredCameraSets[i])
                .WriteBuffer(0, m_deferredCameraDataUBOs[i], 0, sizeof(CameraDataUBO))
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
        m_camera.Initialize(90.f, { 0.f, 1.f, 0.f }, 0.1f, 3000.f);
        m_camera.CalculateProjectionMatrix(m_swapchain.extent.width, m_swapchain.extent.height);
    }

    m_renderCommandBuffers = m_context.commandPool.Allocate(MAX_FRAMES_IN_FLIGHT);


}

bool dae::DeferredApplication::ShouldClose() const
{
    return m_window.ShouldClose();
}

void dae::DeferredApplication::PollEvents() const
{
    m_window.PollEvents();
}

void dae::DeferredApplication::Update()
{
    m_timer.Update();
    m_camera.Update(m_timer, m_window);

    UpdateViewProjectionUBO(m_syncManager.GetCurrentFrameIndex());
}

void dae::DeferredApplication::DrawFrame()
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

void dae::DeferredApplication::Cleanup()
{
    vkDeviceWaitIdle(m_context.logicalDevice.handle);

    m_resizeDeletionQueue.Flush();
    m_applicationDeletionQueue.Flush();
}

void dae::DeferredApplication::HandleResize()
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
    //TODO cleanup this code
    DepthHelpers::CreateDepthImage(m_context, m_swapchain, m_gBuffer.depthImage);

    ImageBuilder imageBuilder{};
    imageBuilder
        .SetWidth(m_swapchain.extent.width)
        .SetHeight(m_swapchain.extent.height)
        .SetMipLevels(1)
        .SetFormat(VK_FORMAT_R8G8B8A8_UNORM) //TODO: check format 
        .SetTiling(VK_IMAGE_TILING_OPTIMAL)
        .SetUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
        .SetAspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
        .SetAllocUsage(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE)
        .Build(m_context, m_gBuffer.albedoImage);

    //Normal Image
    imageBuilder = {};
    imageBuilder
        .SetWidth(m_swapchain.extent.width)
        .SetHeight(m_swapchain.extent.height)
        .SetMipLevels(1)
        .SetFormat(VK_FORMAT_R16G16B16A16_SFLOAT)
        .SetTiling(VK_IMAGE_TILING_OPTIMAL)
        .SetUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
        .SetAspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
        .SetAllocUsage(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE)
        .Build(m_context, m_gBuffer.normalImage);


    // Metallic & Roughness Image
    imageBuilder = {};
    imageBuilder
        .SetWidth(m_swapchain.extent.width)
        .SetHeight(m_swapchain.extent.height)
        .SetMipLevels(1)
        .SetFormat(VK_FORMAT_R8G8B8A8_UNORM)
        .SetTiling(VK_IMAGE_TILING_OPTIMAL)
        .SetUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
        .SetAspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
        .SetAllocUsage(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE)
        .Build(m_context, m_gBuffer.metallicRoughnessImage);

    DescriptorSetWriter deferredWriter{};
    deferredWriter
        .SetTarget(&m_deferredDescriptorSet)
        .WriteImage(0, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_gBuffer.albedoImage, m_sampler)
        .WriteImage(1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_gBuffer.depthImage, m_sampler)
        .WriteImage(2, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_gBuffer.normalImage, m_sampler)
        .WriteImage(3, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_gBuffer.metallicRoughnessImage, m_sampler)
        .UpdateTarget(m_context);

    m_resizeDeletionQueue.PushDeletor([&]() {m_gBuffer.Destroy(m_context);});

    //Recreate RenderFinishedSemaphores
    m_syncManager.RecreateRenderFinishedSemaphores(m_context, static_cast<uint32_t>(m_swapchain.images.size()));

    //Adjust camere aspect ratios
    m_camera.CalculateProjectionMatrix(m_swapchain.extent.width, m_swapchain.extent.height);
}


void dae::DeferredApplication::RecordRenderCommandBuffer(CommandBuffer& commandBuffer, uint32_t imageIndex)
{
    commandBuffer.Begin();

    Image& swapchainImage = m_swapchain.images[imageIndex];
    VkClearValue colorClearValue{ .color = {0.f, 0.f, 0.f, 1.f} };
    VkClearValue depthClearValue{ .depthStencil = {1.f, 0} };

    //Insert image memory barriers
    ImageHelpers::TransitionImageLayout(commandBuffer, m_gBuffer.depthImage, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_2_NONE, VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
        VK_ACCESS_2_NONE, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);

    ImageHelpers::TransitionImageLayout(commandBuffer, swapchainImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_2_NONE, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_NONE, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);

    ImageHelpers::TransitionImageLayout(commandBuffer, m_gBuffer.albedoImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_2_NONE, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_NONE, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);

    ImageHelpers::TransitionImageLayout(commandBuffer, m_gBuffer.normalImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_2_NONE, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_NONE, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);

    ImageHelpers::TransitionImageLayout(commandBuffer, m_gBuffer.metallicRoughnessImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_2_NONE, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_NONE, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);

    //Dynamic States
    VkViewport viewport{};
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = static_cast<float>(m_swapchain.extent.width);
    viewport.height = static_cast<float>(m_swapchain.extent.height);
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    VkRect2D scissor{};
    scissor.offset = { 0,0 };
    scissor.extent = m_swapchain.extent;

    uint32_t currentFrame = m_syncManager.GetCurrentFrameIndex();

    //Depth Prepass
    VkRenderingAttachmentInfo prepassAttachment{};
    prepassAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    prepassAttachment.imageView = m_gBuffer.depthImage.view;
    prepassAttachment.imageLayout = m_gBuffer.depthImage.layout;
    prepassAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    prepassAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    prepassAttachment.clearValue = depthClearValue;

    VkRenderingInfo prepassRenderInfo{};
    prepassRenderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    prepassRenderInfo.renderArea = scissor;
    prepassRenderInfo.layerCount = 1;
    prepassRenderInfo.colorAttachmentCount = 0;
    prepassRenderInfo.pColorAttachments = nullptr;
    prepassRenderInfo.pDepthAttachment = &prepassAttachment;
    prepassRenderInfo.pStencilAttachment = nullptr;

    vkCmdBeginRendering(commandBuffer.handle, &prepassRenderInfo);

    //Bind prepass pipeline & states
    vkCmdBindPipeline(commandBuffer.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, m_prepassPipeline.handle);
    vkCmdSetViewport(commandBuffer.handle, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer.handle, 0, 1, &scissor);
    vkCmdBindDescriptorSets(commandBuffer.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, m_geometryPipelineLayout.handle, 0, 1, &m_globalDescriptorSet.set, 0, nullptr);
    vkCmdBindDescriptorSets(commandBuffer.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, m_geometryPipelineLayout.handle, 1, 1, &m_frameDescriptorSets[currentFrame].set, 0, nullptr);

    //Render scene
    m_scene.RecordDraw(commandBuffer, m_prepassPipeline);

    vkCmdEndRendering(commandBuffer.handle);

    //Insert barrier so prepass is done writing before the main rendering starts
    ImageHelpers::TransitionImageLayout(commandBuffer, m_gBuffer.depthImage, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT);

    //-----------------------------------------------------------
    //Set attachments for Geometry Pipeline
    //Albedo Image
    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = m_gBuffer.albedoImage.view;
    colorAttachment.imageLayout = m_gBuffer.albedoImage.layout;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue = colorClearValue;

    VkRenderingAttachmentInfo normalAttachment{};
    normalAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    normalAttachment.imageView = m_gBuffer.normalImage.view;
    normalAttachment.imageLayout = m_gBuffer.normalImage.layout;
    normalAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    normalAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkRenderingAttachmentInfo metallicRoughnessAttachment{};
    metallicRoughnessAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    metallicRoughnessAttachment.imageView = m_gBuffer.metallicRoughnessImage.view;
    metallicRoughnessAttachment.imageLayout = m_gBuffer.metallicRoughnessImage.layout;
    metallicRoughnessAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    metallicRoughnessAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    std::vector<VkRenderingAttachmentInfo> colorAttachments{
        colorAttachment,
        normalAttachment,
        metallicRoughnessAttachment
    };

    VkRenderingAttachmentInfo depthAttachment{};
    depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depthAttachment.imageView = m_gBuffer.depthImage.view;
    depthAttachment.imageLayout = m_gBuffer.depthImage.layout;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.clearValue = depthClearValue;

    VkRenderingInfo renderInfo{};
    renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderInfo.renderArea = VkRect2D{ VkOffset2D{0,0}, m_swapchain.extent };
    renderInfo.layerCount = 1;
    renderInfo.colorAttachmentCount = colorAttachments.size();
    renderInfo.pColorAttachments = colorAttachments.data();
    renderInfo.pDepthAttachment = &depthAttachment;
    renderInfo.pStencilAttachment = nullptr;

    vkCmdBeginRendering(commandBuffer.handle, &renderInfo);

    //Bind data
    vkCmdBindPipeline(commandBuffer.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, m_geometryPipeline.handle);
    vkCmdSetViewport(commandBuffer.handle, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer.handle, 0, 1, &scissor);
    vkCmdBindDescriptorSets(commandBuffer.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, m_geometryPipelineLayout.handle, 0, 1, &m_globalDescriptorSet.set, 0, nullptr);
    vkCmdBindDescriptorSets(commandBuffer.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, m_geometryPipelineLayout.handle, 1, 1, &m_frameDescriptorSets[currentFrame].set, 0, nullptr);

    //Render scene
    m_scene.RecordDraw(commandBuffer, m_geometryPipeline);

    vkCmdEndRendering(commandBuffer.handle);

    //Transition G Buffer images to SHADER READ ONLY
    ImageHelpers::TransitionImageLayout(commandBuffer, m_gBuffer.albedoImage, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_2_SHADER_SAMPLED_READ_BIT);
    ImageHelpers::TransitionImageLayout(commandBuffer, m_gBuffer.normalImage, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_2_SHADER_SAMPLED_READ_BIT);
    ImageHelpers::TransitionImageLayout(commandBuffer, m_gBuffer.depthImage, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_ACCESS_2_SHADER_SAMPLED_READ_BIT);
    ImageHelpers::TransitionImageLayout(commandBuffer, m_gBuffer.metallicRoughnessImage, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_2_SHADER_SAMPLED_READ_BIT);

    // Deferred Pipeline Rendering
    VkClearValue deferredClearValue{ .color = {1.f, 0.f, 1.f, 1.f} };


    VkRenderingAttachmentInfo deferredColorAttachment{};
    deferredColorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    deferredColorAttachment.imageView = swapchainImage.view;
    deferredColorAttachment.imageLayout = swapchainImage.layout;
    deferredColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    deferredColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    deferredColorAttachment.clearValue = deferredClearValue;

    VkRenderingInfo deferredRenderInfo{};
    deferredRenderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    deferredRenderInfo.renderArea = VkRect2D{ VkOffset2D{0,0}, m_swapchain.extent };
    deferredRenderInfo.layerCount = 1;
    deferredRenderInfo.colorAttachmentCount = 1;
    deferredRenderInfo.pColorAttachments = &deferredColorAttachment;
    
    vkCmdBeginRendering(commandBuffer.handle, &deferredRenderInfo);

    //Bind data
    vkCmdBindPipeline(commandBuffer.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, m_deferredPipeline.handle);
    vkCmdSetViewport(commandBuffer.handle, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer.handle, 0, 1, &scissor);
    vkCmdBindDescriptorSets(commandBuffer.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, m_deferredPipelineLayout.handle, 0, 1, &m_deferredDescriptorSet.set, 0, nullptr);
    vkCmdBindDescriptorSets(commandBuffer.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, m_deferredPipelineLayout.handle, 1, 1, &m_deferredCameraSets[currentFrame].set, 0, nullptr);
    vkCmdDraw(commandBuffer.handle, 3, 1, 0, 0);
    vkCmdEndRendering(commandBuffer.handle);

    //Present image memory barrier
    ImageHelpers::TransitionImageLayout(commandBuffer, swapchainImage, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_NONE,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_2_NONE);

    commandBuffer.End();
}

void dae::DeferredApplication::UpdateViewProjectionUBO(uint32_t currentImage)
{
    ViewProjectionUBO ubo{
        m_camera.viewMatrix,
        m_camera.projectionMatrix
    };

    memcpy(m_viewProjUBOs[currentImage].mappedData, &ubo, sizeof(ubo));
}
