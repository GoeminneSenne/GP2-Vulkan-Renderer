#pragma once

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <set>
#include <cstring>
#include <optional>
#include <algorithm>
#include <array>
#include <chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#define GLFW_INCLUDE_VULKAN
#include <fstream>
#include <limits>
#include <GLFW/glfw3.h>

//VMA includes
#define VMA_IMPLEMENTATION
#define VMA_STATS_STRING_ENABLED 1
#define VMA_DEBUG_LOG_FORMAT
#include <vk_mem_alloc.h>

//STB Image include
//TODO: this should be handled in CMAKE in the end
#define STB_IMAGE_IMPLEMENTATION
#include "../inc/stb_image.h"

//TINY OBJ include
//TODO: should be replaced later on
#define TINYOBJLOADER_IMPLEMENTATION
#include "../inc/tiny_obj_loader.h"

//Project Includes
////////////////////
#include "Window.h"
#include "Vertex.h"
#include "VulkanLoader.h"
#include "Instance.h"
#include "InstanceBuilder.h"
#include "PhysicalDeviceSelector.h"
#include "PhysicalDevice.h"
#include "Common.h"
#include "QueueFamilyIndices.h"
#include "LogicalDevice.h"
#include "LogicalDeviceBuilder.h"
#include "Swapchain.h"
#include "Image.h"
#include "Sampler.h"
#include "SamplerBuilder.h"
#include "ShaderLoader.h"
#include "Shader.h"
#include "CommandBuffer.h"
#include "CommandPool.h"
#include "Buffer.h"
#include "BufferHelpers.h"
#include "ImageBuilder.h"
#include "ImageHelpers.h"
#include "DescriptorSetLayout.h"
#include "DescriptorSetLayoutBuilder.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "DescriptorSetWriter.h"
#include "Fence.h"
#include "Semaphore.h"
#include "SwapchainHelpers.h"
#include "RenderPass.h"
#include "RenderPassBuilder.h"
#include "GraphicsPipeline.h"
#include "GraphicsPipelineLayout.h"


using namespace dae;

constexpr static uint32_t WIDTH = 800;
constexpr static uint32_t HEIGHT = 600;

const std::string MODEL_PATH = "resources/models/viking_room.obj";
const std::string TEXTURE_PATH = "resources/textures/viking_room.png";

constexpr static int MAX_FRAMES_IN_FLIGHT = 3;

const std::vector<const char*> validationLayers{ "VK_LAYER_KHRONOS_validation" };

const std::vector<const char*> deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef NDEBUG
constexpr static bool enableValidationLayers = false;
#else
constexpr static bool enableValidationLayers = true;
#endif


struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

/*
// Triangle demo
const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4
};
*/

class HelloTriangleApplication {
public:
    HelloTriangleApplication()
        : m_window(WIDTH, HEIGHT, "Vulkan")
    {}


    void run() {
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    Window m_window;
    Instance m_instance{};
    PhysicalDevice m_physicalDevice{};
    LogicalDevice m_logicalDevice{};

    Swapchain m_swapchain{};

    RenderPass m_renderPass{};
    
	DescriptorSetLayout m_descriptorSetLayout{};
    DescriptorPool m_descriptorPool;
    std::vector<DescriptorSet> m_descriptorSets;
    
    GraphicsPipelineLayout m_graphicsPipelineLayout{};
    GraphicsPipeline m_graphicsPipeline{};

	//VkPipelineLayout pipelineLayout{};
    //VkPipeline graphicsPipeline{};

    CommandPool m_commandPool{};
    std::vector<CommandBuffer> m_renderCommandBuffers;

    std::vector<Semaphore> m_imageAvailableSemaphores;
    std::vector<Semaphore> m_renderFinishedSemaphores;
    std::vector<Fence> m_inFlightFences;
    uint32_t currentFrame = 0;

    VmaAllocator allocator{};

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    
    Buffer m_vertexBuffer{};
    Buffer m_indexBuffer{};

    std::vector<Buffer> m_uniformBuffers;
    std::vector<void*> uniformBuffersMapped;

    Image m_textureImage{};
    Sampler m_sampler{};

    Image m_depthImage{};

    void initVulkan() 
	{
        CreateInstance();
        PickPhysicalDevice();
        CreateLogicalDevice();
        m_swapchain.Create(m_window, m_instance, m_physicalDevice, m_logicalDevice);
        createRenderPass();
        createDescriptorSetLayout();
    	createGraphicsPipeline();
        m_commandPool.CreateCommandPool(m_physicalDevice, &m_logicalDevice);
    	createVmaAllocator();
        createDepthResources();
        SwapchainHelpers::CreateFramebuffers(m_logicalDevice, m_renderPass, m_depthImage, m_swapchain);
        createTextureImage();
        CreateTextureSampler();
        loadModel();
        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffers();
        CreateDescriptorPool();
        createDescriptorSets();
        m_renderCommandBuffers = m_commandPool.Allocate(MAX_FRAMES_IN_FLIGHT);
    	createSyncObjects();
    }

    void mainLoop() 
	{
        while (!m_window.ShouldClose())
        {
            m_window.PollEvents();
        	drawFrame();
        }

        vkDeviceWaitIdle(m_logicalDevice.device);
    }

    void cleanup() 
	{
        cleanupSwapChain();

        m_sampler.Destroy(m_logicalDevice);
        m_textureImage.Destroy(m_logicalDevice, allocator);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            vmaUnmapMemory(allocator, m_uniformBuffers[i].allocation);
            m_uniformBuffers[i].Destroy(allocator);
        }

        m_descriptorPool.Destroy();
    	m_descriptorSetLayout.Destroy(m_logicalDevice);

        m_indexBuffer.Destroy(allocator);
        m_vertexBuffer.Destroy(allocator);

        vmaDestroyAllocator(allocator);

        m_graphicsPipeline.Destroy(m_logicalDevice);
        m_graphicsPipelineLayout.Destroy(m_logicalDevice);

        m_renderPass.Destroy(m_logicalDevice);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            m_imageAvailableSemaphores[i].Destroy(m_logicalDevice);
            m_inFlightFences[i].Destroy(m_logicalDevice);
        }

        for (const auto& semaphore : m_renderFinishedSemaphores)
        {
            semaphore.Destroy(m_logicalDevice);
        }

        m_commandPool.Destroy();

        m_logicalDevice.Destroy();

        m_instance.Destroy();
        m_window.Destroy();

    }

    void CreateInstance()
    {
        InstanceBuilder instanceBuilder = {};
        instanceBuilder
            .SetApplicationName("Hello Triangle")
            .SetApiVersion(VK_MAKE_VERSION(1, 0, 0))
            .SetEngineName("No Engine")
            .SetEngineVersion(VK_MAKE_VERSION(1, 0, 0))
            .SetApiVersion(VK_API_VERSION_1_0)
            .UseValidation(enableValidationLayers)
			.AddValidationLayers(validationLayers)
            .Build(m_instance);

        m_instance.CreateSurface(m_window);
    }

    void PickPhysicalDevice()
    {
        PhysicalDeviceSelector physicalDeviceSelector = {};
        physicalDeviceSelector
            .AddDeviceExtensions(deviceExtensions)
            .PickPhysicalDevice(m_instance, m_physicalDevice);
    }

    void CreateLogicalDevice()
    {
        LogicalDeviceBuilder logicalDeviceBuilder = {};
        logicalDeviceBuilder
            .EnablebleAnisotropy(true)
            .AddDeviceExtensions(deviceExtensions)
            .UseValidation(true)
            .AddValidationLayers(validationLayers)
            .Build(m_physicalDevice, m_logicalDevice);
    }

    void cleanupSwapChain()
    {
        m_depthImage.Destroy(m_logicalDevice, allocator);
        m_swapchain.Destroy(m_logicalDevice, allocator);
    }

    void recreateSwapChain()
	{
        int width = 0, height = 0;
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(m_window.window, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(m_logicalDevice.device);

        cleanupSwapChain();

        m_swapchain.Create(m_window, m_instance, m_physicalDevice, m_logicalDevice);
        createDepthResources();
        SwapchainHelpers::CreateFramebuffers(m_logicalDevice, m_renderPass, m_depthImage, m_swapchain);


        //Semaphore reconstruction
        //TODO: should be in seperate function or handled elsewhere??
        for (auto& semaphore : m_renderFinishedSemaphores)
        {
            semaphore.Destroy(m_logicalDevice);
            semaphore.Create(m_logicalDevice);
        }
    }

    void createRenderPass()
    {
        RenderPassBuilder renderPassBuilder = {};
        renderPassBuilder
			.AddColorAttachment(m_swapchain.imageFormat)
            .SetDepthAttachment(findDepthFormat())
            .Build(m_logicalDevice, m_renderPass);
    }

    void createDescriptorSetLayout()
    {
        DescriptorSetLayoutBuilder layoutBuilder{};
        layoutBuilder
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
            .AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .Build(m_logicalDevice, m_descriptorSetLayout);
    }

    void createGraphicsPipeline()
    {
        Shader vertexShader, fragmentShader;
        
    	ShaderLoader shaderLoader = {};
        shaderLoader
            .SetFilename("resources/shaders/base_shader.vert.spv")
            .Load(m_logicalDevice, vertexShader);

        shaderLoader
            .SetFilename("resources/shaders/base_shader.frag.spv")
            .Load(m_logicalDevice, fragmentShader);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertexShader.module;
        vertShaderStageInfo.pName = vertexShader.entryPoint.c_str();

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragmentShader.module;
        fragShaderStageInfo.pName = fragmentShader.entryPoint.c_str();

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        auto bindingDescription = Vertex::GetBindingDescription();
        auto attributeDescriptions = Vertex::GetAttributeDescriptions();

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();


        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;


        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
            | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.f;
        colorBlending.blendConstants[1] = 0.f;
        colorBlending.blendConstants[2] = 0.f;
        colorBlending.blendConstants[3] = 0.f;

        std::vector<VkDynamicState> dynamicStates
			= {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout.layout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(m_logicalDevice.device, &pipelineLayoutInfo, nullptr, &m_graphicsPipelineLayout.layout) != VK_SUCCESS)
            throw std::runtime_error("failed to create pipeline layout!");

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = m_graphicsPipelineLayout.layout;
        pipelineInfo.renderPass = m_renderPass.renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;
        pipelineInfo.pDepthStencilState = &depthStencil;

        if (vkCreateGraphicsPipelines(m_logicalDevice.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline.pipeline) != VK_SUCCESS)
            throw std::runtime_error("failed to create graphics pipeline!");

        fragmentShader.Destroy(m_logicalDevice);
        vertexShader.Destroy(m_logicalDevice);
    }

    void createVmaAllocator()
    {
        VmaAllocatorCreateInfo createInfo{};
        createInfo.physicalDevice = m_physicalDevice.physicalDevice;
        createInfo.device = m_logicalDevice.device;
        createInfo.instance = m_instance.instance;
        createInfo.flags = 0;
        
        LOG_VULKAN_ERROR(
            vmaCreateAllocator(&createInfo, &allocator),
			"failed to create VMA allocator!"
        );
    }

    void createDepthResources()
    {
        VkFormat depthFormat = findDepthFormat();

        VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (hasStencilComponent(depthFormat))
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
            .Build(allocator, m_logicalDevice, m_depthImage);

        CommandBuffer commandBuffer = m_commandPool.BeginSingleTimeCommand();
        ImageHelpers::transitionImageLayout(commandBuffer, m_depthImage, 
           VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
           0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
           VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        m_commandPool.EndSingleTimeCommand(commandBuffer);
    }

    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
    {
	    for (VkFormat format : candidates)
	    {
            VkFormatProperties properties;
            vkGetPhysicalDeviceFormatProperties(m_physicalDevice.physicalDevice, format, &properties);

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

    VkFormat findDepthFormat() const
    {
        return findSupportedFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }

    bool hasStencilComponent(VkFormat format) const
    {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    void createTextureImage()
    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight,
            &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels)
            throw std::runtime_error("failed to load texture image!");

        Buffer stagingBuffer;

        BufferHelpers::CreateBuffer(allocator, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            stagingBuffer);

        void* data;
        vmaMapMemory(allocator, stagingBuffer.allocation, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vmaUnmapMemory(allocator, stagingBuffer.allocation);

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
            .Build(allocator, m_logicalDevice, m_textureImage);

        CommandBuffer commandBuffer = m_commandPool.BeginSingleTimeCommand();
        ImageHelpers::transitionImageLayout(commandBuffer, m_textureImage, 
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        m_commandPool.EndSingleTimeCommand(commandBuffer);

        commandBuffer = m_commandPool.BeginSingleTimeCommand();
        BufferHelpers::CopyBufferToImage(commandBuffer, stagingBuffer, m_textureImage);
        m_commandPool.EndSingleTimeCommand(commandBuffer);

        stagingBuffer.Destroy(allocator);

        commandBuffer = m_commandPool.BeginSingleTimeCommand();
        ImageHelpers::GenerateMipmaps(commandBuffer, m_physicalDevice, m_textureImage);
        m_commandPool.EndSingleTimeCommand(commandBuffer);
    }

    void CreateTextureSampler()
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
            .Build(m_physicalDevice, m_logicalDevice, m_sampler);
    }

    void loadModel()
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
                    attrib.vertices[3 * index.vertex_index + 0 ],
                    attrib.vertices[3 * index.vertex_index + 1 ],
                    attrib.vertices[3 * index.vertex_index + 2 ]
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

    //TODO deze 2 functies zijn heel similar
    void createVertexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        Buffer stagingBuffer;

        BufferHelpers::CreateBuffer(allocator, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            stagingBuffer);

        void* data;
        vmaMapMemory(allocator, stagingBuffer.allocation, &data);
        memcpy(data, vertices.data(), (size_t)bufferSize);
        vmaUnmapMemory(allocator, stagingBuffer.allocation);

        BufferHelpers::CreateBuffer(allocator, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VMA_MEMORY_USAGE_AUTO, 0, m_vertexBuffer);


        CommandBuffer commandBuffer = m_commandPool.BeginSingleTimeCommand();
        BufferHelpers::CopyBuffer(commandBuffer, stagingBuffer, m_vertexBuffer, bufferSize);
        m_commandPool.EndSingleTimeCommand(commandBuffer);

        stagingBuffer.Destroy(allocator);
    }

    void createIndexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        Buffer stagingBuffer;

        BufferHelpers::CreateBuffer(allocator, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            stagingBuffer);

        void* data;
        vmaMapMemory(allocator, stagingBuffer.allocation, &data);
        memcpy(data, indices.data(), (size_t)bufferSize);
        vmaUnmapMemory(allocator, stagingBuffer.allocation);

        BufferHelpers::CreateBuffer(allocator, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VMA_MEMORY_USAGE_AUTO, 0,
            m_indexBuffer);

        CommandBuffer commandBuffer = m_commandPool.BeginSingleTimeCommand();
        BufferHelpers::CopyBuffer(commandBuffer, stagingBuffer, m_indexBuffer, bufferSize);
        m_commandPool.EndSingleTimeCommand(commandBuffer);

        stagingBuffer.Destroy(allocator);
    }

    void createUniformBuffers()
    {
	    m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
	        VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	        BufferHelpers::CreateBuffer(allocator, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO,
	                                    VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
	                                    m_uniformBuffers[i]);

            vmaMapMemory(allocator, m_uniformBuffers[i].allocation , &uniformBuffersMapped[i]);
        }
    }

    void CreateDescriptorPool()
    {
        std::vector<VkDescriptorPoolSize> poolSizes;
        poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));
        poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));

        m_descriptorPool.Create(&m_logicalDevice, poolSizes, MAX_FRAMES_IN_FLIGHT);
    }
    
    void createDescriptorSets()
    {
        m_descriptorSets = m_descriptorPool.Allocate(m_descriptorSetLayout, MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            DescriptorSetWriter writer = {};
            writer
        	    .SetTarget(&m_descriptorSets[i])
                .WriteBuffer(0, m_uniformBuffers[i], 0, sizeof(UniformBufferObject))
                .WriteImage(1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_textureImage, m_sampler)
                .UpdateTarget(m_logicalDevice);
        }
    }

    void UpdateUniformBuffer(uint32_t currentImage) const
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

    //Record Render Command Buffer
    void recordRenderCommandBuffer(CommandBuffer& commandBuffer, uint32_t imageIndex) const
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

        VkBuffer vertexBuffers[] = {m_vertexBuffer.buffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(commandBuffer.commandBuffer, m_indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipelineLayout.layout, 0, 1, &m_descriptorSets[currentFrame].set, 0, nullptr);
        
    	vkCmdDrawIndexed(commandBuffer.commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        vkCmdEndRenderPass(commandBuffer.commandBuffer);

        commandBuffer.End();
    }

    void createSyncObjects()
    {
        m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        
    	m_renderFinishedSemaphores.resize(m_swapchain.images.size());

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i )
        {
            m_imageAvailableSemaphores[i].Create(m_logicalDevice);
            m_inFlightFences[i].Create(m_logicalDevice);
        }

        for (size_t i = 0; i < m_swapchain.images.size(); ++i)
        {
            m_renderFinishedSemaphores[i].Create(m_logicalDevice);
        }

    }

    void drawFrame()
    {
        m_inFlightFences[currentFrame].Wait(m_logicalDevice);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(m_logicalDevice.device, m_swapchain.swapchain, UINT64_MAX, m_imageAvailableSemaphores[currentFrame].Get(), VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            throw std::runtime_error("failed to acquire swap chain image!");

        UpdateUniformBuffer(currentFrame);

        m_inFlightFences[currentFrame].Reset(m_logicalDevice);

        ///Record
        vkResetCommandBuffer(m_renderCommandBuffers[currentFrame].commandBuffer, 0);
        recordRenderCommandBuffer(m_renderCommandBuffers[currentFrame], imageIndex);

        ///Submit
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[currentFrame].Get()};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_renderCommandBuffers[currentFrame].commandBuffer;

        VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[imageIndex].Get()};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        m_logicalDevice.graphicsQueue.Submit(submitInfo, m_inFlightFences[currentFrame]);

        ///Present
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {m_swapchain.swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        result = m_logicalDevice.presentQueue.Present(presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window.wasResized)
        {
            m_window.wasResized = false;
        	recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
            throw std::runtime_error("failed to present swap chain images!");

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}