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

const std::vector<const char*> validationLayers{ "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

namespace dae
{
    class DeferredApplication final
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

        //--- General Members ------------------------------
        Window m_window{};
        Timer m_timer{};
        Camera m_camera{};
        Context m_context{};
        Swapchain m_swapchain{};
        std::vector<CommandBuffer> m_renderCommandBuffers;
        DescriptorPool m_descriptorPool;
        SyncManager m_syncManager{};
        Scene m_scene{};
        Sampler m_sampler{};
        GBuffer m_gBuffer{};
        DeletionQueue m_applicationDeletionQueue{};
        DeletionQueue m_resizeDeletionQueue{};


        //--- Geometry Pipeline -----------------------------
        PipelineLayout m_geometryPipelineLayout{};
        GraphicsPipeline m_geometryPipeline{};
        GraphicsPipeline m_prepassPipeline{};

        //Global Data
        DescriptorSetLayout m_texturesSetLayout{};
        DescriptorSet m_texturesSet{};

        //Frame Specific Data
        DescriptorSetLayout m_viewProjSetLayout{};
        std::vector<DescriptorSet> m_viewProjSets{};
        std::vector<UniformBuffer> m_viewProjUBOs;

        //--- Deferred Pipeline -----------------------------
        PipelineLayout m_deferredPipelineLayout{};
        GraphicsPipeline m_deferredPipeline{};

        DescriptorSetLayout m_gBufferSetLayout{};
        DescriptorSet m_gBufferSet{};

        DescriptorSetLayout m_camDataSetLayout{};
        std::vector<DescriptorSet> m_camDataSets{};
        std::vector<UniformBuffer> m_camDataUBOs;

        DescriptorSetLayout m_lightsSetLayout{};
        DescriptorSet m_lightsSet{};
        Buffer m_pointLightsBuffer{};
        Buffer m_dirLightsBuffer{};

        //--- Blitting Pass -------------------------------
        Image m_hdrBackBuffer{};

        PipelineLayout m_blittingPassPipelineLayout{};
        GraphicsPipeline m_blittingPassPipeline{};
        DescriptorSetLayout m_blittingPassSetLayout{};
        DescriptorSet m_blittingPassSet{};


    };
}