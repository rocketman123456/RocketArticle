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

        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device;

        VkQueue graphicsQueue;
        VkQueue presentQueue;

        VkSwapchainKHR swapChain;
        Vec<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        Vec<VkImageView> swapChainImageViews;
        Vec<VkFramebuffer> swapChainFramebuffers;

        VkRenderPass renderPass;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;

        VkCommandPool commandPool;
        Vec<VkCommandBuffer> commandBuffers;

        Vec<VkSemaphore> imageAvailableSemaphores;
        Vec<VkSemaphore> renderFinishedSemaphores;
        Vec<VkFence> inFlightFences;
        Vec<VkFence> imagesInFlight;
        size_t currentFrame = 0;

        bool framebufferResized = false;
    };
}