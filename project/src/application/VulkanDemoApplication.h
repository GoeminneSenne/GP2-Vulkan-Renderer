#pragma once


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <memory>
#include <glm/gtx/hash.hpp>

#include "../framework/Window.h"
#include "../framework/Context.h"
#include "../framework/Common.h"
#include "../framework/Swapchain.h"
#include "../framework/Image.h"
#include "../framework/Sampler.h"
#include "../framework/CommandBuffer.h"
#include "../framework/Buffer.h"
#include "../framework/DescriptorSet.h"
#include "../framework/RenderPass.h"
#include "../framework/GraphicsPipeline.h"
#include "../framework/PipelineLayout.h"
#include "../framework/Mesh.h"
#include "../framework/SyncManager.h"
#include "../framework/SwapchainSupportDetails.h"

constexpr static uint32_t WIDTH = 800;
constexpr static uint32_t HEIGHT = 600;

const std::string MODEL_PATH = "resources/models/viking_room.obj";
const std::string TEXTURE_PATH = "resources/textures/viking_room.png";

const std::vector<const char*> validationLayers{ "VK_LAYER_KHRONOS_validation" };

const std::vector<const char*> deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

namespace dae
{
	class VulkanDemoApplication final
	{
	public:
		void Init();
		bool ShouldClose() const;
		void PollEvents() const;
		void DrawFrame();
		void Cleanup();

	private:
        //void CreateWindow();
        //void CreateContext();
        //void CreateRenderPass();
        //void CreateDescriptorSetLayout();
        //void CreateGraphicsPipeline();
        void CreateDepthResources();
        //void CreateTextureSampler();
        //void CreateUniformBuffers();
        //void CreateDescriptorPool();
        //void CreateDescriptorSets();

        void CleanupSwapchain();
        void RecreateSwapchain();
        void RecordRenderCommandBuffer(CommandBuffer& commandBuffer, uint32_t imageIndex);

        VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
        VkFormat FindDepthFormat() const;
        bool HasStencilComponent(VkFormat format) const;
        void UpdateUniformBuffer(uint32_t currentImage) const;

		Window m_window{};
        Context m_context{};

        Swapchain m_swapchain{};

        RenderPass m_renderPass{};
        DescriptorSetLayout m_descriptorSetLayout{};
        DescriptorPool m_descriptorPool;
        std::vector<DescriptorSet> m_descriptorSets;
        PipelineLayout m_graphicsPipelineLayout{};
        GraphicsPipeline m_graphicsPipeline{};

        std::vector<CommandBuffer> m_renderCommandBuffers;

        SyncManager m_syncManager{};

        std::unique_ptr<Mesh> m_pMesh;

        std::vector<Buffer> m_uniformBuffers;
        std::vector<void*> uniformBuffersMapped;

        Image m_textureImage{};
        Sampler m_sampler{};
        Image m_depthImage{};

        DeletionQueue m_applicationDeletionQueue{};
	};
}
