#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <memory>
#include <glm/gtx/hash.hpp>

#include <framework/Window.h>
#include <framework/Context.h>
#include <framework/Common.h>
#include <framework/Swapchain.h>
#include <framework/Image.h>
#include <framework/Sampler.h>
#include <framework/CommandBuffer.h>
#include <framework/Buffer.h>
#include <framework/DescriptorSet.h>
#include <framework/RenderPass.h>
#include <framework/GraphicsPipeline.h>
#include <framework/PipelineLayout.h>
#include <framework/Mesh.h>
#include <framework/SyncManager.h>
#include <framework/Camera.h>
#include <framework/Timer.h>
#include <framework/SwapchainSupportDetails.h>
#include <framework/GLTFLoader.h>
#include <framework/GpuData.h>

constexpr static uint32_t WIDTH = 800;
constexpr static uint32_t HEIGHT = 600;

const std::string TEXTURE_PATH = "resources/viking_room/viking_room.png";

const std::vector<const char*> validationLayers{ "VK_LAYER_KHRONOS_validation" };

const std::vector<const char*> deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

namespace dae
{
	class RenderPassExampleApplication final
	{
    public:
        void Init();
        bool ShouldClose() const;
        void PollEvents() const;
        void Update();
        void DrawFrame();
        void Cleanup();

    private:

        void HandleResize();
        void RecordRenderCommandBuffer(CommandBuffer& commandBuffer, uint32_t imageIndex);
        void UpdateViewProjectionUBO(uint32_t currentImage);

        Window m_window{};
        Timer m_timer{};
        Camera m_camera{};

        Context m_context{};

        Swapchain m_swapchain{};

        RenderPass m_renderPass{};
        DescriptorSetLayout m_frameDataSetLayout{};
        DescriptorSetLayout m_globalDataSetLayout{};
        DescriptorPool m_descriptorPool;
        DescriptorSet m_globalDescriptorSet{};
        std::vector<DescriptorSet> m_frameDescriptorSets;
        PipelineLayout m_graphicsPipelineLayout{};
        GraphicsPipeline m_graphicsPipeline{};

        std::vector<CommandBuffer> m_renderCommandBuffers;

        SyncManager m_syncManager{};

        Scene m_scene{};

        std::vector<Buffer> m_uniformBuffers;
        std::vector<void*> uniformBuffersMapped;

        Image m_textureImage{};
        Sampler m_sampler{};
        
		Image m_depthImage{};

        DeletionQueue m_applicationDeletionQueue{};
        DeletionQueue m_resizeDeletionQueue{};
	};
}