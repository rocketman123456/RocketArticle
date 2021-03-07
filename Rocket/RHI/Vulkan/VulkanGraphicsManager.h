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
        VkDevice GetDevice() { return m_Device; }
        VkPhysicalDevice GetPhysicalDevice()  { return m_PhysicalDevice; }
        VkSurfaceKHR GetSurface() { return m_Surface; }
        
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

        void RecordCommandBuffer(uint32_t frameIndex);
        void RecordGuiCommandBuffer(uint32_t frameIndex);

        void CleanupSwapChain();
        void RecreateSwapChain();

        void UpdateUniformBuffer(uint32_t currentImage);

    private:
        bool m_VSync = true;
        bool m_IsScenePrepared = false;
        bool m_IsRecreateSwapChain = false;
        GLFWwindow* m_WindowHandle = nullptr;

        //Ref<VulkanUI> m_VulkanUI = nullptr;
        Ref<VulkanDevice> m_LogicalDevice;
        Ref<VulkanSwapChain> m_VulkanSwapChain = nullptr;
        Ref<VulkanPipeline> m_VulkanPipeline = nullptr;
        Ref<VulkanFrameBuffer> m_VulkanFrameBuffer = nullptr;
        Ref<VulkanTexture2D> m_VulkanTexture2D = nullptr;
        Ref<TextureCubeMap> m_EnvironmentCube = nullptr;
        Ref<TextureCubeMap> m_IrradianceCube = nullptr;
        Ref<TextureCubeMap> m_PrefilteredCube = nullptr;

        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        VkSurfaceKHR m_Surface;

        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;
        VkPhysicalDeviceProperties m_DeviceProperties;
        VkPhysicalDeviceFeatures m_DeviceFeatures;
        VkPhysicalDeviceMemoryProperties m_DeviceMemoryProperties;

        VkDevice m_Device;

        VkQueue m_GraphicsQueue;
        VkQueue m_ComputeQueue;
        VkQueue m_PresentQueue;

        VkSwapchainKHR m_SwapChain;
        Vec<VkImage> m_SwapChainImages;
        VkFormat m_SwapChainImageFormat;
        VkExtent2D m_SwapChainExtent;
        Vec<VkImageView> m_SwapChainImageViews;
        Vec<VkFramebuffer> m_SwapChainFramebuffers;

        VkRenderPass m_RenderPass;
        VkRenderPass m_GuiRenderPass;
        VkDescriptorSetLayout m_DescriptorSetLayout;
        VkPipelineLayout m_PipelineLayout;
        VkPipeline m_GraphicsPipeline;
        VkPipelineCache m_PipelineCache;

        VkImage m_BRDFImage = VK_NULL_HANDLE;
        VkDeviceMemory m_BRDFImageMemory;
        VkImageView m_BRDFImageView;
        VkSampler m_BRDFSampler;

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

        Vec<Vertex> m_Vertices;
        Vec<uint32_t> m_Indices;
        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;
        VkBuffer m_IndexBuffer;
        VkDeviceMemory m_IndexBufferMemory;

        Vec<VkBuffer> m_UniformBuffers;
        Vec<VkDeviceMemory> m_UniformBuffersMemory;

        VkDescriptorPool m_DescriptorPool;
        Vec<VkDescriptorSet> m_DescriptorSets;

        Vec<VkCommandBuffer> m_CommandBuffers;
        Vec<VkCommandBuffer> m_GuiCommandBuffer;
        Vec<VkCommandBuffer> m_SubmitBuffers;

        Vec<VkSemaphore> m_ImageAvailableSemaphores;
        Vec<VkSemaphore> m_RenderFinishedSemaphores;
        Vec<VkFence> m_InFlightFences;
        Vec<VkFence> m_ImagesInFlight;

        bool m_FramebufferResized = false;
        bool m_SkipCurrentFrame = false;
    };
}