#pragma once
#include "Module/GraphicsManager.h"
#include "Common/Buffer.h"

#include <vulkan/vulkan.hpp>
#include <optional>
#include <set>

struct GLFWwindow;

namespace Rocket
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        Vec<VkSurfaceFormatKHR> formats;
        Vec<VkPresentModeKHR> presentModes;
    };
    
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

        void BeginFrame(const Frame& frame) final;
        void EndFrame(const Frame& frame) final;

        void BeginFrameBuffer(const Frame& frame) final;
        void EndFrameBuffer(const Frame& frame) final;

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
        
    private:
        void drawFrame();

        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSwapChain();
        void createImageViews();
        void createRenderPass();
        void createGraphicsPipeline();
        void createFramebuffers();
        void createCommandPool();
        void createCommandBuffers();
        void createSyncObjects();

        void cleanupSwapChain();
        void recreateSwapChain();

    private:
        bool m_VSync = true;
        GLFWwindow* m_WindowHandle = nullptr;

        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        VkSurfaceKHR m_Surface;

        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device;

        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;

        VkSwapchainKHR m_SwapChain;
        Vec<VkImage> m_SwapChainImages;
        VkFormat m_SwapChainImageFormat;
        VkExtent2D m_SwapChainExtent;
        Vec<VkImageView> m_SwapChainImageViews;
        Vec<VkFramebuffer> m_SwapChainFramebuffers;

        VkRenderPass m_RenderPass;
        VkDescriptorSetLayout m_DescriptorSetLayout;
        VkPipelineLayout m_PipelineLayout;
        VkPipeline m_GraphicsPipeline;

        VkCommandPool m_CommandPool;

        VkImage m_ColorImage;
        VkDeviceMemory m_ColorImageMemory;
        VkImageView m_ColorImageView;

        VkImage m_DepthImage;
        VkDeviceMemory m_DepthImageMemory;
        VkImageView m_DepthImageView;

        uint32_t m_MipLevels;
        VkImage m_TextureImage;
        VkDeviceMemory m_TextureImageMemory;
        VkImageView m_TextureImageView;
        VkSampler m_TextureSampler;

        //Vec<Vertex> m_Vertices;
        //Vec<uint32_t> m_Indices;
        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;
        VkBuffer m_IndexBuffer;
        VkDeviceMemory m_IndexBufferMemory;

        Vec<VkBuffer> m_UniformBuffers;
        Vec<VkDeviceMemory> m_UniformBuffersMemory;

        VkDescriptorPool m_DescriptorPool;
        Vec<VkDescriptorSet> m_DescriptorSets;

        Vec<VkCommandBuffer> m_CommandBuffers;

        Vec<VkSemaphore> m_ImageAvailableSemaphores;
        Vec<VkSemaphore> m_RenderFinishedSemaphores;
        Vec<VkFence> m_InFlightFences;
        Vec<VkFence> m_ImagesInFlight;
        size_t m_CurrentFrame = 0;

        bool m_FramebufferResized = false;
    };
}