#include "VulkanDemoApplication.h"

#include <array>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#include "../../inc/stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../inc/tiny_obj_loader.h"

//VMA includes
#define VMA_IMPLEMENTATION
#define VMA_STATS_STRING_ENABLED 1
#define VMA_DEBUG_LOG_FORMAT
#include <vk_mem_alloc.h>

#include "../framework/DescriptorSet.h"
#include "../framework/Sampler.h"
#include "../framework/RenderPass.h"
#include "../framework/Shader.h"
#include "../framework/SwapchainHelpers.h"
#include "../framework/Vertex.h"
#include "../framework/BufferHelpers.h"
#include "../framework/Framebuffer.h"


void dae::VulkanDemoApplication::Init()
{
    CreateWindow();
	CreateContext();
	m_swapchain.Create(m_window, m_context);
    m_applicationDeletionQueue.PushDeletor([&]() {m_swapchain.Destroy(m_context); });
	CreateRenderPass();
	CreateDescriptorSetLayout();
	CreateGraphicsPipeline();
	CreateDepthResources();
	SwapchainHelpers::CreateFramebuffers(m_context, m_renderPass, m_depthImage, m_swapchain);
	CreateTextureImage();
	CreateTextureSampler();
	LoadModel();
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateUniformBuffers();
	CreateDescriptorPool();
	CreateDescriptorSets();
	m_renderCommandBuffers = m_context.commandPool.Allocate(MAX_FRAMES_IN_FLIGHT);
	CreateSyncObjects();
}

bool dae::VulkanDemoApplication::ShouldClose() const
{
	return m_window.ShouldClose();
}

void dae::VulkanDemoApplication::PollEvents() const
{
	m_window.PollEvents();
}

void dae::VulkanDemoApplication::DrawFrame()
{
    m_inFlightFences[currentFrame].Wait(m_context);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(m_context.logicalDevice.device, m_swapchain.swapchain, UINT64_MAX, m_imageAvailableSemaphores[currentFrame].Get(), VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapchain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("failed to acquire swap chain image!");

    UpdateUniformBuffer(currentFrame);

    m_inFlightFences[currentFrame].Reset(m_context);

    ///Record
    vkResetCommandBuffer(m_renderCommandBuffers[currentFrame].commandBuffer, 0);
    RecordRenderCommandBuffer(m_renderCommandBuffers[currentFrame], imageIndex);

    ///Submit
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[currentFrame].Get() };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_renderCommandBuffers[currentFrame].commandBuffer;

    VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[imageIndex].Get() };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    m_context.logicalDevice.graphicsQueue.Submit(submitInfo, m_inFlightFences[currentFrame]);

    ///Present
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

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

}

void dae::VulkanDemoApplication::Cleanup()
{
	vkDeviceWaitIdle(m_context.logicalDevice.device);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        vmaUnmapMemory(m_context.allocator, m_uniformBuffers[i].allocation);
        m_uniformBuffers[i].Destroy(m_context);
    }

    m_descriptorPool.Destroy(m_context);


    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        m_imageAvailableSemaphores[i].Destroy(m_context);
        m_inFlightFences[i].Destroy(m_context);
    }

    for (const auto& semaphore : m_renderFinishedSemaphores)
    {
        semaphore.Destroy(m_context);
    }

    m_applicationDeletionQueue.Flush();
}

void dae::VulkanDemoApplication::CreateWindow()
{
    m_window.InitWindow(WIDTH, HEIGHT, "Vulkan Demo");
    m_applicationDeletionQueue.PushDeletor([&]() {m_window.Destroy(); });
}

void dae::VulkanDemoApplication::CreateContext()
{
    ContextConfig config{
        enableValidationLayers,
        validationLayers,
        {},
        deviceExtensions
    };

    ContextFactory contextFactory{ config };
    contextFactory.Create(&m_window, m_context);

    m_applicationDeletionQueue.PushDeletor([&]() {m_context.instance.Destroy(); });
    m_applicationDeletionQueue.PushDeletor([&]() {m_context.logicalDevice.Destroy(); });
    m_applicationDeletionQueue.PushDeletor([&]() {m_context.commandPool.Destroy(); });
    m_applicationDeletionQueue.PushDeletor([&]() {vmaDestroyAllocator(m_context.allocator); });
}

void dae::VulkanDemoApplication::CreateRenderPass()
{
    RenderPassBuilder renderPassBuilder = {};
    renderPassBuilder
        .AddColorAttachment(m_swapchain.imageFormat)
        .SetDepthAttachment(FindDepthFormat())
        .Build(m_context, m_renderPass);

    m_applicationDeletionQueue.PushDeletor([&]() {m_renderPass.Destroy(m_context); });
}

void dae::VulkanDemoApplication::CreateDescriptorSetLayout()
{
    DescriptorSetLayoutBuilder layoutBuilder{};
    layoutBuilder
        .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
        .AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        .Build(m_context, m_descriptorSetLayout);

    m_applicationDeletionQueue.PushDeletor([&]() {m_descriptorSetLayout.Destroy(m_context); });

}

void dae::VulkanDemoApplication::CreateGraphicsPipeline()
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
    ImageHelpers::transitionImageLayout(commandBuffer, m_depthImage,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    m_context.commandPool.EndSingleTimeCommand(commandBuffer);
}

void dae::VulkanDemoApplication::CreateTextureImage()
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight,
        &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels)
        throw std::runtime_error("failed to load texture image!");

    Buffer stagingBuffer;
    BufferHelpers::CreateStaging(m_context, imageSize, stagingBuffer);

    BufferHelpers::Upload(m_context, pixels, imageSize, stagingBuffer);

    /*
    void* data;
    vmaMapMemory(m_context.allocator, stagingBuffer.allocation, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vmaUnmapMemory(m_context.allocator, stagingBuffer.allocation);
	*/

    stbi_image_free(pixels);

    uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

    ImageBuilder imageBuilder = {};
    imageBuilder
        .SetWidth(texWidth)
        .SetHeight(texHeight)
        .SetMipLevels(mipLevels)
        .SetFormat(VK_FORMAT_R8G8B8A8_SRGB)
        .SetTiling(VK_IMAGE_TILING_OPTIMAL)
        .SetUsageFlags(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
        .SetAspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
        .SetAllocUsage(VMA_MEMORY_USAGE_AUTO)
        .SetAllocFlags(VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT)
        .Build(m_context, m_textureImage);

    m_applicationDeletionQueue.PushDeletor([&]() {m_textureImage.Destroy(m_context); });


    CommandBuffer commandBuffer = m_context.commandPool.BeginSingleTimeCommand();
    ImageHelpers::transitionImageLayout(commandBuffer, m_textureImage,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    m_context.commandPool.EndSingleTimeCommand(commandBuffer);

    commandBuffer = m_context.commandPool.BeginSingleTimeCommand();
    BufferHelpers::CopyBufferToImage(commandBuffer, stagingBuffer, m_textureImage);
    m_context.commandPool.EndSingleTimeCommand(commandBuffer);

    stagingBuffer.Destroy(m_context);

    commandBuffer = m_context.commandPool.BeginSingleTimeCommand();
    ImageHelpers::GenerateMipmaps(commandBuffer, m_context, m_textureImage);
    m_context.commandPool.EndSingleTimeCommand(commandBuffer);
}

void dae::VulkanDemoApplication::CreateTextureSampler()
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

void dae::VulkanDemoApplication::LoadModel()
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, MODEL_PATH.c_str()))
    {
        throw std::runtime_error(err);
    }

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = { 1.f, 1.f, 1.f };

            if (!uniqueVertices.contains(vertex))
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }
}

void dae::VulkanDemoApplication::CreateVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    Buffer stagingBuffer;
    BufferHelpers::CreateStaging(m_context, bufferSize, stagingBuffer);

    BufferHelpers::Upload(m_context, vertices.data(), bufferSize, stagingBuffer);

    /*
    void* data;
    vmaMapMemory(m_context.allocator, stagingBuffer.allocation, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vmaUnmapMemory(m_context.allocator, stagingBuffer.allocation);
	*/

    BufferHelpers::CreateVertex(m_context, bufferSize, m_vertexBuffer);
	m_applicationDeletionQueue.PushDeletor([&]() {m_vertexBuffer.Destroy(m_context); });



    CommandBuffer commandBuffer = m_context.commandPool.BeginSingleTimeCommand();
    BufferHelpers::CopyBuffer(commandBuffer, stagingBuffer, m_vertexBuffer, bufferSize);
    m_context.commandPool.EndSingleTimeCommand(commandBuffer);

    stagingBuffer.Destroy(m_context);
}

void dae::VulkanDemoApplication::CreateIndexBuffer()
{
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    Buffer stagingBuffer;
    BufferHelpers::CreateStaging(m_context, bufferSize, stagingBuffer);

    BufferHelpers::Upload(m_context, indices.data(), bufferSize, stagingBuffer);
    /*
     
    void* data;
    vmaMapMemory(m_context.allocator, stagingBuffer.allocation, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vmaUnmapMemory(m_context.allocator, stagingBuffer.allocation);
     */

	BufferHelpers::CreateIndex(m_context, bufferSize, m_indexBuffer);
    m_applicationDeletionQueue.PushDeletor([&]() {m_indexBuffer.Destroy(m_context); });


    CommandBuffer commandBuffer = m_context.commandPool.BeginSingleTimeCommand();
    BufferHelpers::CopyBuffer(commandBuffer, stagingBuffer, m_indexBuffer, bufferSize);
    m_context.commandPool.EndSingleTimeCommand(commandBuffer);

    stagingBuffer.Destroy(m_context);
}

void dae::VulkanDemoApplication::CreateUniformBuffers()
{
    m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);
        BufferHelpers::CreateUniform(m_context, bufferSize, m_uniformBuffers[i]);

        vmaMapMemory(m_context.allocator, m_uniformBuffers[i].allocation, &uniformBuffersMapped[i]);
    }
}

void dae::VulkanDemoApplication::CreateDescriptorPool()
{
    std::vector<VkDescriptorPoolSize> poolSizes;
    poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));
    poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));

    m_descriptorPool.Create(m_context, poolSizes, MAX_FRAMES_IN_FLIGHT);
}

void dae::VulkanDemoApplication::CreateDescriptorSets()
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

void dae::VulkanDemoApplication::CreateSyncObjects()
{
    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    m_renderFinishedSemaphores.resize(m_swapchain.images.size());

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        m_imageAvailableSemaphores[i].Create(m_context);
        m_inFlightFences[i].Create(m_context);
    }

    for (size_t i = 0; i < m_swapchain.images.size(); ++i)
    {
        m_renderFinishedSemaphores[i].Create(m_context);
    }
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


    //Semaphore reconstruction
    //TODO: should be in seperate function or handled elsewhere??
    for (auto& semaphore : m_renderFinishedSemaphores)
    {
        semaphore.Destroy(m_context);
        semaphore.Create(m_context);
    }
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

    VkBuffer vertexBuffers[] = { m_vertexBuffer.buffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer.commandBuffer, m_indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipelineLayout.layout, 0, 1, &m_descriptorSets[currentFrame].set, 0, nullptr);

    vkCmdDrawIndexed(commandBuffer.commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

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

void dae::VulkanDemoApplication::UpdateUniformBuffer(uint32_t currentImage) const
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
    ubo.view = glm::lookAt(glm::vec3(2.f, 2.f, 2.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f));
    ubo.proj = glm::perspective(glm::radians(45.0f), (float)m_swapchain.extent.width / (float)m_swapchain.extent.height, 0.1f, 10.f);
    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}
