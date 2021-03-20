#pragma once
#include "Module/GraphicsManager.h"
#include "Vulkan/Render/VulkanFunction.h"
#include "Vulkan/Render/VulkanDevice.h"
#include "Vulkan/Render/VulkanSwapChain.h"
#include "Vulkan/Render/VulkanPipeline.h"
#include "Vulkan/Render/VulkanFrameBuffer.h"
#include "Vulkan/Render/VulkanShader.h"
#include "Vulkan/Render/VulkanTexture.h"
#include "Vulkan/Render/VulkanUI.h"
#include "Common/Buffer.h"
// TODO : finish scene structure
#include "Scene/Component/Model.h"

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Rocket
{   
    class VulkanGraphicsManager : implements GraphicsManager
    {
    public:
        RUNTIME_MODULE_TYPE(VulkanGraphicsManager);
        VulkanGraphicsManager() = default;
        virtual ~VulkanGraphicsManager() = default;

        virtual int Initialize() final;
        virtual void Finalize() final;

        virtual void Tick(Timestep ts) final;

        void SetPipelineState(const Ref<PipelineState>& pipelineState, const Frame& frame) final;

        void BeginPass(const Frame& frame) final {}
        void EndPass(const Frame& frame) final {}

        void BeginCompute() final {}
        void EndCompute() final {}

        void GenerateCubeMaps() final;
        void GenerateBRDFLUT() final;

        void BeginFrame(const Frame& frame) final;
        void EndFrame(const Frame& frame) final;

        void BeginFrameBuffer(const Frame& frame) final;
        void EndFrameBuffer(const Frame& frame) final;

        void BeginScene(const Scene& scene) final;
        void EndScene() final;

        void SetPerFrameConstants(const DrawFrameContext& context) final;
        void SetPerBatchConstants(const DrawBatchContext& context) final;
        void SetLightInfo(const DrawFrameContext& context) final;

        void Present() final;

        void DrawBatch(const Frame& frame) final;
        void DrawFullScreenQuad() final;

        bool OnWindowResize(EventPtr& e) final;

        // For Debug
        void DrawPoint(const Point3D& point, const Vector3f& color) final;
        void DrawPointSet(const Point3DSet& point_set, const Vector3f& color) final;
        void DrawPointSet(const Point3DSet& point_set, const Matrix4f& trans, const Vector3f& color) final;

        void DrawLine(const Point3D& from, const Point3D& to, const Vector3f& color) final;
        void DrawLine(const Point3DList& vertices, const Vector3f& color) final;
        void DrawLine(const Point3DList& vertices, const Matrix4f& trans, const Vector3f& color) final;

        void DrawTriangle(const Point3DList& vertices, const Vector3f& color) final;
        void DrawTriangle(const Point3DList& vertices, const Matrix4f& trans, const Vector3f& color) final;
        void DrawTriangleStrip(const Point3DList& vertices, const Vector3f& color) final;

        // Getter
        VkDevice GetDevice() { return device_; }
        VkPhysicalDevice GetPhysicalDevice()  { return physical_device_; }
        VkSurfaceKHR GetSurface() { return surface_; }
        
    private:
        void CreateInstance();
        void SetupDebugMessenger();
        void CreateSurface();
        void PickPhysicalDevice();

        void CreateLogicalDevice();
        void CreateSwapChain();
        void CreateGraphicsPipeline();
        void CreateFramebuffers();

        void InitGui();

        void CreateTextureImage();

        void LoadModel();

        void CreateVertexBuffer();
        void CreateIndexBuffer();

        void CreateUniformBuffers();
        void CreateDescriptorPool();
        void CreateDescriptorSets();

        void CreateCommandBuffers();
        void CreateSyncObjects();

        void RecordCommandBuffer(uint32_t frame_index);
        void RecordGuiCommandBuffer(uint32_t frame_index);

        void CleanupSwapChain();
        void RecreateSwapChain();

        void UpdateUniformBuffer(uint32_t currentImage);

    private:
        bool vsync_ = true;
        bool is_scene_prepared_ = false;
        bool is_recreate_swapchain_ = false;
        GLFWwindow* window_handle_ = nullptr;

        Ref<VulkanDevice> logical_device_;
        Ref<VulkanSwapchain> vulkan_swapchain_ = nullptr;
        Ref<VulkanPipeline> vulkan_pipeline_ = nullptr;
        Ref<VulkanFrameBuffer> vulkan_framebuffer_ = nullptr;
        Ref<VulkanTexture2D> vulkan_texture_2d_ = nullptr;
        Ref<TextureCubeMap> environment_cube_ = nullptr;
        Ref<TextureCubeMap> irradiance_cube_ = nullptr;
        Ref<TextureCubeMap> prefiltered_cube_ = nullptr;

        VkInstance instance_;
        VkDebugUtilsMessengerEXT debug_messenger_;
        VkSurfaceKHR surface_;

        VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
        VkSampleCountFlagBits msaa_samples_ = VK_SAMPLE_COUNT_1_BIT;
        VkPhysicalDeviceProperties device_properties_;
        VkPhysicalDeviceFeatures device_features_;
        VkPhysicalDeviceMemoryProperties device_memory_properties_;

        VkDevice device_;

        VkQueue graphics_queue_;
        VkQueue compute_queue_;
        VkQueue present_queue_;

        VkSwapchainKHR swap_chain_;
        Vec<VkImage> swapchain_images_;
        VkFormat swapchain_image_format_;
        VkExtent2D swapchain_extent_;
        Vec<VkImageView> swapchain_image_views_;
        Vec<VkFramebuffer> swapchain_framebuffers_;

        VkRenderPass render_pass_;
        VkRenderPass gui_render_pass_;
        VkDescriptorSetLayout descriptor_set_layout_;
        VkPipelineLayout pipeline_layout_;
        VkPipeline graphics_pipeline_;
        VkPipelineCache pipeline_cache_;

        VkImage brdf_image_ = VK_NULL_HANDLE;
        VkDeviceMemory brdf_image_memory_;
        VkImageView brdf_image_view_;
        VkSampler brdf_sampler_;

        VkCommandPool command_pool_;

        VkImage color_image_;
        VkDeviceMemory color_image_memory_;
        VkImageView color_image_view_;

        VkImage depth_image_;
        VkDeviceMemory depth_image_memory_;
        VkImageView depth_image_view_;

        uint32_t mip_levels_;
        VkImage texture_image_;
        VkDeviceMemory texture_image_memory_;
        VkImageView texture_image_view_;
        VkSampler texture_sampler_;

        Vec<Vertex> vertices_;
        Vec<uint32_t> indices_;
        VkBuffer vertex_buffer_;
        VkDeviceMemory vertex_buffer_memory_;
        VkBuffer index_buffer_;
        VkDeviceMemory index_buffer_memory_;

        Vec<VkBuffer> uniform_buffers_;
        Vec<VkDeviceMemory> uniform_buffers_memory_;

        VkDescriptorPool descriptor_pool_;
        Vec<VkDescriptorSet> descriptor_sets_;

        Vec<VkCommandBuffer> command_buffers_;
        Vec<VkCommandBuffer> gui_command_buffer_;
        Vec<VkCommandBuffer> submit_buffers_;

        Vec<VkSemaphore> image_available_semaphores_;
        Vec<VkSemaphore> render_finished_semaphores_;
        Vec<VkFence> in_flight_fences_;
        Vec<VkFence> images_in_flight_;

        bool framebuffer_resized_ = false;
        bool skip_current_frame_ = false;
    };
}