#include "Vulkan/VulkanGraphicsManager.h"
#include "Module/WindowManager.h"
#include "Module/AssetLoader.h"
#include "Module/Application.h"

#include <GLFW/glfw3.h>
#include <tiny_obj_loader.h>

using namespace Rocket;

GraphicsManager* Rocket::GetGraphicsManager() { return new VulkanGraphicsManager(); }

// TODO : change this configuration to config file
Vec<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
Vec<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
#ifdef RK_DEBUG
bool enableValidationLayers = true;
#else
bool enableValidationLayers = false;
#endif

//--------------------------------------------------------------------//
//----- Debug Message ------------------------------------------------//
//--------------------------------------------------------------------//

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
        func(instance, debugMessenger, pAllocator);
}

//--------------------------------------------------------------------//
//--------------------------------------------------------------------//
//--------------------------------------------------------------------//

int VulkanGraphicsManager::Initialize()
{
    auto config = g_Application->GetConfig();

    bool ret = GraphicsManager::Initialize();
    if (ret)
    {
        RK_GRAPHICS_ERROR("GraphicsManager Initialize Failed");
        return ret;
    }

    m_WindowHandle = static_cast<GLFWwindow*>(g_WindowManager->GetNativeWindow());

    // Must be : 1, 2, 4, 8, 16, 32, 64
    m_MsaaSamples = (VkSampleCountFlagBits)config->GetConfigInfo<uint32_t>("Graphics", "msaa_sample_count");

    CreateInstance();
    SetupDebugMessenger();
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();

    CreateSwapChain();
    CreateGraphicsPipeline();
    CreateFramebuffers();
    InitGui();

    CreateSyncObjects();

    CreateUniformBuffers();

    return 0;
}

void VulkanGraphicsManager::Finalize()
{
    vkDeviceWaitIdle(m_Device);

    GraphicsManager::Finalize();

    CleanupSwapChain();

    m_VulkanSwapChain->Finalize();
    m_VulkanPipeline->Finalize();

    vkDestroyPipelineCache(m_Device, m_PipelineCache, nullptr);

    // Destroy Uniform Buffers
    for (size_t i = 0; i < m_SwapChainImages.size(); i++)
    {
        vkDestroyBuffer(m_Device, m_UniformBuffers[i], nullptr);
        vkFreeMemory(m_Device, m_UniformBuffersMemory[i], nullptr);
    }

    // Destroy SyncObjects
    for (size_t i = 0; i < m_MaxFrameInFlight; i++)
    {
        vkDestroySemaphore(m_Device, m_RenderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(m_Device, m_ImageAvailableSemaphores[i], nullptr);
        vkDestroyFence(m_Device, m_InFlightFences[i], nullptr);
    }

    m_LogicalDevice->Finalize();

    if (enableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    vkDestroyInstance(m_Instance, nullptr);
}

void VulkanGraphicsManager::CleanupSwapChain()
{
    //vkFreeCommandBuffers(m_Device, m_CommandPool, static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
    vkFreeCommandBuffers(m_Device, m_CommandPool, static_cast<uint32_t>(m_GuiCommandBuffer.size()), m_GuiCommandBuffer.data());

    m_VulkanFrameBuffer->Finalize();
    //m_VulkanPipeline->Finalize();
    //m_VulkanSwapChain->Finalize();
    m_VulkanUI->Finalize();

    RK_GRAPHICS_TRACE("CleanupSwapChain");
}

void VulkanGraphicsManager::RecreateSwapChain()
{
    RK_GRAPHICS_TRACE("RecreateSwapChain");
    m_IsScenePrepared = false;

    int width = 0, height = 0;
    glfwGetFramebufferSize(m_WindowHandle, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(m_WindowHandle, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_Device);

    CleanupSwapChain();

    CreateSwapChain();

    InitGui();
    CreateFramebuffers();
    CreateCommandBuffers();

    //for (uint32_t i = 0; i < m_MaxFrameInFlight; ++i)
    //{
    //    RecordCommandBuffer(i);
    //}
    
    vkDeviceWaitIdle(m_Device);

    m_ImagesInFlight.resize(m_SwapChainImages.size(), VK_NULL_HANDLE);

    m_IsScenePrepared = true;
}

//--------------------------------------------------------------------//
//--------------------------------------------------------------------//
//--------------------------------------------------------------------//

void VulkanGraphicsManager::CreateInstance()
{
    if (enableValidationLayers && !CheckValidationLayerSupport())
        RK_GRAPHICS_ERROR("validation layers requested, but not available!");

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Rocket";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Rocket Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = GetRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
        RK_GRAPHICS_ERROR("failed to create instance!");
}

void VulkanGraphicsManager::SetupDebugMessenger()
{
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);

    VK_CHECK(CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger));
}

void VulkanGraphicsManager::CreateSurface()
{
    VK_CHECK(glfwCreateWindowSurface(m_Instance, m_WindowHandle, nullptr, &m_Surface));
}

void VulkanGraphicsManager::PickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

    if (deviceCount == 0)
        RK_GRAPHICS_ERROR("failed to find GPUs with Vulkan support!");

    Vec<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        if (IsDeviceSuitable(device, m_Surface))
        {
            m_PhysicalDevice = device;
            // TODO : use config file
            m_MsaaSamples = GetMaxUsableSampleCount(m_PhysicalDevice);
            //m_MsaaSamples = VK_SAMPLE_COUNT_4_BIT;
            break;
        }
    }

    if (m_PhysicalDevice == VK_NULL_HANDLE)
        RK_GRAPHICS_ERROR("failed to find a suitable GPU!");

    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_DeviceProperties);
    vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_DeviceFeatures);
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_DeviceMemoryProperties);
}

void VulkanGraphicsManager::CreateLogicalDevice()
{
    if (!m_LogicalDevice)
    {
        m_LogicalDevice = CreateRef<VulkanDevice>(m_PhysicalDevice, m_Surface);
    }
    m_LogicalDevice->Initialize();

    VkPhysicalDeviceFeatures enabledFeatures{};
    if (m_DeviceFeatures.samplerAnisotropy) {
        enabledFeatures.samplerAnisotropy = VK_TRUE;
    }
    Vec<const char*> enabledExtensions{};
    VK_CHECK(m_LogicalDevice->CreateLogicalDevice(enabledFeatures, enabledExtensions, enableValidationLayers, validationLayers));

    m_Device = m_LogicalDevice->logicalDevice;
    m_CommandPool = m_LogicalDevice->commandPool;
    m_GraphicsQueue = m_LogicalDevice->graphicsQueue;
    m_ComputeQueue = m_LogicalDevice->computeQueue;
    m_PresentQueue = m_LogicalDevice->presentQueue;
}

void VulkanGraphicsManager::CreateSwapChain()
{
    if (!m_VulkanSwapChain)
    {
        m_VulkanSwapChain = CreateRef<VulkanSwapChain>();
        m_VulkanSwapChain->SetWindowHandle(m_WindowHandle);
    }
    m_VulkanSwapChain->Connect(m_Instance, m_PhysicalDevice, m_Device, m_Surface);
    m_VulkanSwapChain->Initialize(true);
    
    m_SwapChain = m_VulkanSwapChain->swapChain;
    m_SwapChainImages.clear();
    m_SwapChainImageViews.clear();
    m_SwapChainImageFormat = m_VulkanSwapChain->colorFormat;
    m_SwapChainExtent = m_VulkanSwapChain->extent;
    for (auto buffer : m_VulkanSwapChain->buffers)
    {
        m_SwapChainImages.push_back(buffer.image);
        m_SwapChainImageViews.push_back(buffer.view);
    }
}

void VulkanGraphicsManager::CreateGraphicsPipeline()
{
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
    pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    VK_CHECK(vkCreatePipelineCache(m_Device, &pipelineCacheCreateInfo, nullptr, &m_PipelineCache));

    if (!m_VulkanPipeline)
    {
        m_VulkanPipeline = CreateRef<VulkanPipeline>();
        m_VulkanPipeline->SetWindowHandle(m_WindowHandle);
        m_VulkanPipeline->SetMsaaSample(m_MsaaSamples);
    }
    m_VulkanPipeline->Connect(m_Instance, m_PhysicalDevice, m_Device, m_Surface);
    m_VulkanPipeline->Initialize();

    m_RenderPass = m_VulkanPipeline->renderPass;
    m_GuiRenderPass = m_VulkanPipeline->guiRenderPass;
    m_DescriptorSetLayout = m_VulkanPipeline->descriptorSetLayout;
    m_PipelineLayout = m_VulkanPipeline->pipelineLayout;
    m_GraphicsPipeline = m_VulkanPipeline->graphicsPipeline;
}

void VulkanGraphicsManager::CreateFramebuffers()
{
    if (!m_VulkanFrameBuffer)
    {
        m_VulkanFrameBuffer = CreateRef<VulkanFrameBuffer>();
        m_VulkanFrameBuffer->SetWindowHandle(m_WindowHandle);
        m_VulkanFrameBuffer->SetMsaaSample(m_MsaaSamples);
    }
    m_VulkanFrameBuffer->Connect(m_Device, m_PhysicalDevice, m_Surface, m_RenderPass, m_SwapChainImageViews);
    m_VulkanFrameBuffer->Initialize();

    m_ColorImage = m_VulkanFrameBuffer->colorImage;
    m_ColorImageMemory = m_VulkanFrameBuffer->colorImageMemory;
    m_ColorImageView = m_VulkanFrameBuffer->colorImageView;

    m_DepthImage = m_VulkanFrameBuffer->depthImage;
    m_DepthImageMemory = m_VulkanFrameBuffer->depthImageMemory;
    m_DepthImageView = m_VulkanFrameBuffer->depthImageView;

    m_SwapChainFramebuffers.resize(m_VulkanFrameBuffer->swapChainFramebuffers.size());
    for (size_t i = 0; i < m_SwapChainFramebuffers.size(); ++i)
    {
        m_SwapChainFramebuffers[i] = m_VulkanFrameBuffer->swapChainFramebuffers[i];
    }
}

void VulkanGraphicsManager::CreateSyncObjects()
{
    m_ImageAvailableSemaphores.resize(m_MaxFrameInFlight);
    m_RenderFinishedSemaphores.resize(m_MaxFrameInFlight);
    m_InFlightFences.resize(m_MaxFrameInFlight);
    m_ImagesInFlight.resize(m_SwapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < m_MaxFrameInFlight; i++)
    {
        VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]));
        VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]));
        VK_CHECK(vkCreateFence(m_Device, &fenceInfo, nullptr, &m_InFlightFences[i]));
    }
}

void VulkanGraphicsManager::InitGui()
{
    if (!m_VulkanUI)
    {
        m_VulkanUI = CreateRef<VulkanUI>();
        m_VulkanUI->SetWindowHandle(m_WindowHandle);
    }
    m_VulkanUI->Connect(m_Instance, m_LogicalDevice, m_RenderPass, m_GraphicsQueue, m_PipelineCache, m_MsaaSamples, m_MaxFrameInFlight);
    m_VulkanUI->Initialize();
    m_VulkanUI->UpdataOverlay(m_SwapChainExtent.width, m_SwapChainExtent.height);
    m_VulkanUI->PrepareUI();
}

void VulkanGraphicsManager::CreateCommandBuffers()
{
    m_CommandBuffers.resize(m_SwapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

    VK_CHECK(vkAllocateCommandBuffers(m_Device, &allocInfo, m_CommandBuffers.data()));
    
    m_GuiCommandBuffer.resize(m_SwapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfoGui{};
    allocInfoGui.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfoGui.commandPool = m_CommandPool;
    allocInfoGui.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfoGui.commandBufferCount = static_cast<uint32_t>(m_GuiCommandBuffer.size());

    VK_CHECK(vkAllocateCommandBuffers(m_Device, &allocInfoGui, m_GuiCommandBuffer.data()));
}

//--------------------------------------------------------------------//
//----- Scene --------------------------------------------------------//
//--------------------------------------------------------------------//

void VulkanGraphicsManager::LoadModel()
{
    tinyobj::attrib_t attrib;
    Vec<tinyobj::shape_t> shapes;
    Vec<tinyobj::material_t> materials;
    String warn, err;

    String asset_path = g_AssetLoader->GetAssetPath();
    String model_path = "Models/viking_room.obj";
    String full_path = asset_path + model_path;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, full_path.c_str()))
    {
        RK_CORE_ERROR("{}", warn + err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};

            vertex.pos = glm::vec3({
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
                });

            vertex.texCoord = glm::vec2({
                attrib.texcoords[2 * index.texcoord_index + 0],
                attrib.texcoords[2 * index.texcoord_index + 1]
                });

            vertex.color = glm::vec3({ 1.0f, 1.0f, 1.0f });

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
                m_Vertices.push_back(vertex);
            }

            m_Indices.push_back(uniqueVertices[vertex]);
        }
    }
}

void VulkanGraphicsManager::CreateVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(m_Device, m_PhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, m_Vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(m_Device, stagingBufferMemory);

    CreateBuffer(m_Device, m_PhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);

    CopyBuffer(m_Device, m_CommandPool, m_GraphicsQueue, stagingBuffer, m_VertexBuffer, bufferSize);

    vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
    vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
}

void VulkanGraphicsManager::CreateIndexBuffer()
{
    VkDeviceSize bufferSize = sizeof(m_Indices[0]) * m_Indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(m_Device, m_PhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, m_Indices.data(), (size_t)bufferSize);
    vkUnmapMemory(m_Device, stagingBufferMemory);

    CreateBuffer(m_Device, m_PhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

    CopyBuffer(m_Device, m_CommandPool, m_GraphicsQueue, stagingBuffer, m_IndexBuffer, bufferSize);

    vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
    vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
}

void VulkanGraphicsManager::CreateTextureImage()
{
    m_VulkanTexture2D = CreateRef<VulkanTexture2D>();
    m_VulkanTexture2D->LoadFromFile("Models/viking_room.png", VK_FORMAT_R8G8B8A8_SRGB, m_LogicalDevice, m_GraphicsQueue);

    m_MipLevels = m_VulkanTexture2D->mipLevels;
    m_TextureImage = m_VulkanTexture2D->image;
    m_TextureImageMemory = m_VulkanTexture2D->deviceMemory;
    m_TextureImageView = m_VulkanTexture2D->view;
    m_TextureSampler = m_VulkanTexture2D->sampler;

    if (m_EnvironmentCube) {
        m_EnvironmentCube->Finalize();
        m_IrradianceCube->Finalize();
        m_PrefilteredCube->Finalize();
    }
    String filename = "Textures/environments/papermill.ktx";
    RK_GRAPHICS_INFO("Loading environment from {}", filename);
    m_EnvironmentCube = CreateRef<TextureCubeMap>();
    m_EnvironmentCube->LoadFromFile(filename, VK_FORMAT_R16G16B16A16_SFLOAT, m_LogicalDevice, m_GraphicsQueue);
    GenerateCubeMaps();
}

void VulkanGraphicsManager::CreateUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    m_UniformBuffers.resize(m_SwapChainImages.size());
    m_UniformBuffersMemory.resize(m_SwapChainImages.size());

    for (size_t i = 0; i < m_SwapChainImages.size(); i++)
    {
        CreateBuffer(
            m_Device,
            m_PhysicalDevice,
            bufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            m_UniformBuffers[i],
            m_UniformBuffersMemory[i]
        );
    }
}

void VulkanGraphicsManager::CreateDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(m_SwapChainImages.size());
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(m_SwapChainImages.size());

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(m_SwapChainImages.size());

    VK_CHECK(vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &m_DescriptorPool));
}

void VulkanGraphicsManager::CreateDescriptorSets()
{
    Vec<VkDescriptorSetLayout> layouts(m_SwapChainImages.size(), m_DescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_DescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(m_SwapChainImages.size());
    allocInfo.pSetLayouts = layouts.data();

    m_DescriptorSets.resize(m_SwapChainImages.size());
    VK_CHECK(vkAllocateDescriptorSets(m_Device, &allocInfo, m_DescriptorSets.data()));

    for (size_t i = 0; i < m_SwapChainImages.size(); i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_UniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = m_TextureImageView;
        imageInfo.sampler = m_TextureSampler;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = m_DescriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = m_DescriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(m_Device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

//--------------------------------------------------------------------//
//----- Tick ---------------------------------------------------------//
//--------------------------------------------------------------------//

void VulkanGraphicsManager::Tick(Timestep ts)
{
    GraphicsManager::Tick(ts);
}

void VulkanGraphicsManager::BeginScene(const Scene& scene)
{
    GraphicsManager::BeginScene(scene);

    LoadModel();

    CreateVertexBuffer();
    CreateIndexBuffer();

    CreateTextureImage();

    CreateDescriptorPool();
    CreateDescriptorSets();

    CreateCommandBuffers();

    for (uint32_t i = 0; i < m_MaxFrameInFlight; ++i)
    {
        RecordCommandBuffer(i);
    }

    m_IsScenePrepared = true;
}

void VulkanGraphicsManager::EndScene()
{
    GraphicsManager::EndScene();

    if (m_IsScenePrepared)
    {
        vkFreeCommandBuffers(m_Device, m_CommandPool, static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
        m_CommandBuffers.clear();
        m_CommandBuffers.resize(m_MaxFrameInFlight);

        // Clear DescriptorPool / DescriptorSet
        vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);

        // Clear BRDF
        vkDestroySampler(m_Device, m_BRDFSampler, nullptr);
        vkDestroyImageView(m_Device, m_BRDFImageView, nullptr);
        vkDestroyImage(m_Device, m_BRDFImage, nullptr);
        vkFreeMemory(m_Device, m_BRDFImageMemory, nullptr);

        // Clear Model Data
        m_VulkanTexture2D->Finalize();
        m_VulkanTexture2D.reset();

        m_EnvironmentCube->Finalize();
        m_IrradianceCube->Finalize();
        m_PrefilteredCube->Finalize();
        m_EnvironmentCube.reset();
        m_IrradianceCube.reset();
        m_PrefilteredCube.reset();

        vkDestroyBuffer(m_Device, m_IndexBuffer, nullptr);
        vkFreeMemory(m_Device, m_IndexBufferMemory, nullptr);

        vkDestroyBuffer(m_Device, m_VertexBuffer, nullptr);
        vkFreeMemory(m_Device, m_VertexBufferMemory, nullptr);
    }
    m_IsScenePrepared = false;
}

void VulkanGraphicsManager::UpdateUniformBuffer(uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), m_SwapChainExtent.width / (float)m_SwapChainExtent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    void* data;
    vkMapMemory(m_Device, m_UniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(m_Device, m_UniformBuffersMemory[currentImage]);
}

void VulkanGraphicsManager::SetPipelineState(const Ref<PipelineState>& pipelineState, const Frame& frame)
{
}

void VulkanGraphicsManager::RecordCommandBuffer(uint32_t frameIndex)
{
    // Record Command Buffer
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VK_CHECK(vkBeginCommandBuffer(m_CommandBuffers[frameIndex], &beginInfo));

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_RenderPass;
    renderPassInfo.framebuffer = m_SwapChainFramebuffers[frameIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = m_SwapChainExtent;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(m_CommandBuffers[frameIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    {
        VkViewport viewport{};
        viewport.width = (float)m_SwapChainExtent.width;
        viewport.height = (float)m_SwapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(m_CommandBuffers[frameIndex], 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.extent = { (uint32_t)m_SwapChainExtent.width, (uint32_t)m_SwapChainExtent.height };
        vkCmdSetScissor(m_CommandBuffers[frameIndex], 0, 1, &scissor);

        vkCmdBindPipeline(m_CommandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
        VkBuffer vertexBuffers[] = { m_VertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(m_CommandBuffers[frameIndex], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(m_CommandBuffers[frameIndex], m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(m_CommandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSets[frameIndex], 0, nullptr);
        vkCmdDrawIndexed(m_CommandBuffers[frameIndex], static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
    }
    vkCmdEndRenderPass(m_CommandBuffers[frameIndex]);

    VK_CHECK(vkEndCommandBuffer(m_CommandBuffers[frameIndex]));
}

void VulkanGraphicsManager::RecordGuiCommandBuffer(uint32_t frameIndex)
{
    // Record And Submit Gui Command
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VK_CHECK(vkBeginCommandBuffer(m_GuiCommandBuffer[frameIndex], &beginInfo));

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_GuiRenderPass;
    renderPassInfo.framebuffer = m_SwapChainFramebuffers[frameIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = m_SwapChainExtent;

    vkCmdBeginRenderPass(m_GuiCommandBuffer[frameIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    {
        VkViewport viewport{};
        viewport.width = (float)m_SwapChainExtent.width;
        viewport.height = (float)m_SwapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(m_GuiCommandBuffer[frameIndex], 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.extent = { (uint32_t)m_SwapChainExtent.width, (uint32_t)m_SwapChainExtent.height };
        vkCmdSetScissor(m_GuiCommandBuffer[frameIndex], 0, 1, &scissor);

        m_VulkanUI->Draw(m_GuiCommandBuffer[frameIndex]);
    }
    vkCmdEndRenderPass(m_GuiCommandBuffer[frameIndex]);

    VK_CHECK(vkEndCommandBuffer(m_GuiCommandBuffer[frameIndex]));
}

void VulkanGraphicsManager::BeginFrame(const Frame& frame)
{
    if (!m_IsScenePrepared)
        return;
    
    // Reset RecreateSwapChain
    m_IsRecreateSwapChain = false;

    //RK_GRAPHICS_TRACE("Begin Frame");
    //RK_CORE_TRACE("width : {}, height : {}", m_SwapChainExtent.width, m_SwapChainExtent.height);
    m_VulkanUI->UpdataOverlay(m_SwapChainExtent.width, m_SwapChainExtent.height);
    m_VulkanUI->PrepareUI();

    vkWaitForFences(m_Device, 1, &m_InFlightFences[m_CurrentFrameIndex], VK_TRUE, UINT64_MAX);
    VkResult acquireResult = m_VulkanSwapChain->AcquireNextImage(m_ImageAvailableSemaphores[m_CurrentFrameIndex], &m_FrameIndex);
    
    if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR) {
        m_IsRecreateSwapChain = true;
        RecreateSwapChain();
        return;
    }
    else if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
        VK_CHECK(acquireResult);
    }

    if (m_ImagesInFlight[m_FrameIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(m_Device, 1, &m_ImagesInFlight[m_FrameIndex], VK_TRUE, UINT64_MAX);
    }
    m_ImagesInFlight[m_FrameIndex] = m_InFlightFences[m_CurrentFrameIndex];

    UpdateUniformBuffer(m_FrameIndex);
    
    RecordCommandBuffer(m_FrameIndex);
    RecordGuiCommandBuffer(m_FrameIndex);
}

void VulkanGraphicsManager::EndFrame(const Frame& frame)
{
    if (!m_IsScenePrepared)
        return;
    if(m_IsRecreateSwapChain)
        return;
    
    Vec<VkCommandBuffer> commandBuffers = {
        m_CommandBuffers[m_FrameIndex],
        m_GuiCommandBuffer[m_FrameIndex],
    };

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &m_ImageAvailableSemaphores[m_CurrentFrameIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &m_RenderFinishedSemaphores[m_CurrentFrameIndex];
    submitInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    submitInfo.pCommandBuffers = commandBuffers.data();
    submitInfo.pWaitDstStageMask = waitStages;

    vkResetFences(m_Device, 1, &m_InFlightFences[m_CurrentFrameIndex]);
    VK_CHECK(vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFences[m_CurrentFrameIndex]));

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[m_CurrentFrameIndex];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_SwapChain;
    presentInfo.pImageIndices = &m_FrameIndex;

    VkResult presentResult = vkQueuePresentKHR(m_PresentQueue, &presentInfo);

    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR || m_FramebufferResized) {
        m_FramebufferResized = false;
        m_IsRecreateSwapChain = true;
        RecreateSwapChain();
        return;
    }
    else if (presentResult != VK_SUCCESS) {
        VK_CHECK(presentResult);
    }

    m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_MaxFrameInFlight;
    //RK_GRAPHICS_TRACE("End Frame");
}

void VulkanGraphicsManager::BeginFrameBuffer(const Frame& frame)
{
}

void VulkanGraphicsManager::EndFrameBuffer(const Frame& frame)
{
}

void VulkanGraphicsManager::SetPerFrameConstants(const DrawFrameContext& context)
{
}

void VulkanGraphicsManager::SetPerBatchConstants(const DrawBatchContext& context)
{
}

void VulkanGraphicsManager::SetLightInfo(const DrawFrameContext& context)
{
}

void VulkanGraphicsManager::DrawBatch(const Frame& frame)
{
}

void VulkanGraphicsManager::Present()
{
    
}

void VulkanGraphicsManager::GenerateCubeMaps()
{
    enum Target { IRRADIANCE = 0, PREFILTEREDENV = 1 };

    for (uint32_t target = 0; target < PREFILTEREDENV + 1; target++)
    {
        TextureCubeMap cubemap;

        auto tStart = std::chrono::high_resolution_clock::now();

        VkFormat format;
        int32_t dim;

        switch (target) {
        case IRRADIANCE:
            format = VK_FORMAT_R32G32B32A32_SFLOAT;
            dim = 64;
            break;
        case PREFILTEREDENV:
            format = VK_FORMAT_R16G16B16A16_SFLOAT;
            dim = 512;
            break;
        };

        const uint32_t numMips = static_cast<uint32_t>(floor(log2(dim))) + 1;

        // Create target cubemap
        {
            // Image
            VkImageCreateInfo imageCI{};
            imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageCI.imageType = VK_IMAGE_TYPE_2D;
            imageCI.format = format;
            imageCI.extent.width = dim;
            imageCI.extent.height = dim;
            imageCI.extent.depth = 1;
            imageCI.mipLevels = numMips;
            imageCI.arrayLayers = 6;
            imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
            imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageCI.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            imageCI.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
            VK_CHECK(vkCreateImage(m_Device, &imageCI, nullptr, &cubemap.image));
            VkMemoryRequirements memReqs;
            vkGetImageMemoryRequirements(m_Device, cubemap.image, &memReqs);
            VkMemoryAllocateInfo memAllocInfo{};
            memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            memAllocInfo.allocationSize = memReqs.size;
            memAllocInfo.memoryTypeIndex = m_LogicalDevice->GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            VK_CHECK(vkAllocateMemory(m_Device, &memAllocInfo, nullptr, &cubemap.deviceMemory));
            VK_CHECK(vkBindImageMemory(m_Device, cubemap.image, cubemap.deviceMemory, 0));

            // View
            VkImageViewCreateInfo viewCI{};
            viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewCI.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
            viewCI.format = format;
            viewCI.subresourceRange = {};
            viewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewCI.subresourceRange.levelCount = numMips;
            viewCI.subresourceRange.layerCount = 6;
            viewCI.image = cubemap.image;
            VK_CHECK(vkCreateImageView(m_Device, &viewCI, nullptr, &cubemap.view));

            // Sampler
            VkSamplerCreateInfo samplerCI{};
            samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerCI.magFilter = VK_FILTER_LINEAR;
            samplerCI.minFilter = VK_FILTER_LINEAR;
            samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerCI.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerCI.minLod = 0.0f;
            samplerCI.maxLod = static_cast<float>(numMips);
            samplerCI.maxAnisotropy = 1.0f;
            samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
            VK_CHECK(vkCreateSampler(m_Device, &samplerCI, nullptr, &cubemap.sampler));
        }

        // FB, Att, RP, Pipe, etc.
        VkAttachmentDescription attDesc{};
        // Color attachment
        attDesc.format = format;
        attDesc.samples = VK_SAMPLE_COUNT_1_BIT;
        attDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        VkAttachmentReference colorReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

        VkSubpassDescription subpassDescription{};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &colorReference;

        // Use subpass dependencies for layout transitions
        std::array<VkSubpassDependency, 2> dependencies;
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        // Renderpass
        VkRenderPassCreateInfo renderPassCI{};
        renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCI.attachmentCount = 1;
        renderPassCI.pAttachments = &attDesc;
        renderPassCI.subpassCount = 1;
        renderPassCI.pSubpasses = &subpassDescription;
        renderPassCI.dependencyCount = 2;
        renderPassCI.pDependencies = dependencies.data();
        VkRenderPass renderpass;
        VK_CHECK(vkCreateRenderPass(m_Device, &renderPassCI, nullptr, &renderpass));

        struct Offscreen {
            VkImage image;
            VkImageView view;
            VkDeviceMemory memory;
            VkFramebuffer framebuffer;
        } offscreen;

        // Create offscreen framebuffer
        {
            // Image
            VkImageCreateInfo imageCI{};
            imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageCI.imageType = VK_IMAGE_TYPE_2D;
            imageCI.format = format;
            imageCI.extent.width = dim;
            imageCI.extent.height = dim;
            imageCI.extent.depth = 1;
            imageCI.mipLevels = 1;
            imageCI.arrayLayers = 1;
            imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
            imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageCI.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            VK_CHECK(vkCreateImage(m_Device, &imageCI, nullptr, &offscreen.image));
            VkMemoryRequirements memReqs;
            vkGetImageMemoryRequirements(m_Device, offscreen.image, &memReqs);
            VkMemoryAllocateInfo memAllocInfo{};
            memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            memAllocInfo.allocationSize = memReqs.size;
            memAllocInfo.memoryTypeIndex = m_LogicalDevice->GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            VK_CHECK(vkAllocateMemory(m_Device, &memAllocInfo, nullptr, &offscreen.memory));
            VK_CHECK(vkBindImageMemory(m_Device, offscreen.image, offscreen.memory, 0));

            // View
            VkImageViewCreateInfo viewCI{};
            viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewCI.format = format;
            viewCI.flags = 0;
            viewCI.subresourceRange = {};
            viewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewCI.subresourceRange.baseMipLevel = 0;
            viewCI.subresourceRange.levelCount = 1;
            viewCI.subresourceRange.baseArrayLayer = 0;
            viewCI.subresourceRange.layerCount = 1;
            viewCI.image = offscreen.image;
            VK_CHECK(vkCreateImageView(m_Device, &viewCI, nullptr, &offscreen.view));

            // Framebuffer
            VkFramebufferCreateInfo framebufferCI{};
            framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCI.renderPass = renderpass;
            framebufferCI.attachmentCount = 1;
            framebufferCI.pAttachments = &offscreen.view;
            framebufferCI.width = dim;
            framebufferCI.height = dim;
            framebufferCI.layers = 1;
            VK_CHECK(vkCreateFramebuffer(m_Device, &framebufferCI, nullptr, &offscreen.framebuffer));

            VkCommandBuffer layoutCmd = m_LogicalDevice->CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
            VkImageMemoryBarrier imageMemoryBarrier{};
            imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imageMemoryBarrier.image = offscreen.image;
            imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            imageMemoryBarrier.srcAccessMask = 0;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
            vkCmdPipelineBarrier(layoutCmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
            m_LogicalDevice->FlushCommandBuffer(layoutCmd, m_GraphicsQueue, true);
        }

        // Descriptors
        VkDescriptorSetLayout descriptorsetlayout;
        VkDescriptorSetLayoutBinding setLayoutBinding = { 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr };
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCI{};
        descriptorSetLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCI.pBindings = &setLayoutBinding;
        descriptorSetLayoutCI.bindingCount = 1;
        VK_CHECK(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCI, nullptr, &descriptorsetlayout));

        // Descriptor Pool
        VkDescriptorPoolSize poolSize = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 };
        VkDescriptorPoolCreateInfo descriptorPoolCI{};
        descriptorPoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCI.poolSizeCount = 1;
        descriptorPoolCI.pPoolSizes = &poolSize;
        descriptorPoolCI.maxSets = 2;
        VkDescriptorPool descriptorpool;
        VK_CHECK(vkCreateDescriptorPool(m_Device, &descriptorPoolCI, nullptr, &descriptorpool));

        // Descriptor sets
        VkDescriptorSet descriptorset;
        VkDescriptorSetAllocateInfo descriptorSetAllocInfo{};
        descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocInfo.descriptorPool = descriptorpool;
        descriptorSetAllocInfo.pSetLayouts = &descriptorsetlayout;
        descriptorSetAllocInfo.descriptorSetCount = 1;
        VK_CHECK(vkAllocateDescriptorSets(m_Device, &descriptorSetAllocInfo, &descriptorset));
        VkWriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.dstSet = descriptorset;
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.pImageInfo = &m_EnvironmentCube->descriptor;
        vkUpdateDescriptorSets(m_Device, 1, &writeDescriptorSet, 0, nullptr);

        struct PushBlockIrradiance {
            glm::mat4 mvp;
            float deltaPhi = (2.0f * float(M_PI)) / 180.0f;
            float deltaTheta = (0.5f * float(M_PI)) / 64.0f;
        } pushBlockIrradiance;

        struct PushBlockPrefilterEnv {
            glm::mat4 mvp;
            float roughness;
            uint32_t numSamples = 32u;
        } pushBlockPrefilterEnv;

        // Pipeline layout
        VkPipelineLayout pipelinelayout;
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

        switch (target) {
        case IRRADIANCE:
            pushConstantRange.size = sizeof(PushBlockIrradiance);
            break;
        case PREFILTEREDENV:
            pushConstantRange.size = sizeof(PushBlockPrefilterEnv);
            break;
        };

        VkPipelineLayoutCreateInfo pipelineLayoutCI{};
        pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCI.setLayoutCount = 1;
        pipelineLayoutCI.pSetLayouts = &descriptorsetlayout;
        pipelineLayoutCI.pushConstantRangeCount = 1;
        pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;
        VK_CHECK(vkCreatePipelineLayout(m_Device, &pipelineLayoutCI, nullptr, &pipelinelayout));

        // Pipeline
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCI{};
        inputAssemblyStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyStateCI.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        VkPipelineRasterizationStateCreateInfo rasterizationStateCI{};
        rasterizationStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateCI.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationStateCI.cullMode = VK_CULL_MODE_NONE;
        rasterizationStateCI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizationStateCI.lineWidth = 1.0f;

        VkPipelineColorBlendAttachmentState blendAttachmentState{};
        blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blendAttachmentState.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlendStateCI{};
        colorBlendStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendStateCI.attachmentCount = 1;
        colorBlendStateCI.pAttachments = &blendAttachmentState;

        VkPipelineDepthStencilStateCreateInfo depthStencilStateCI{};
        depthStencilStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilStateCI.depthTestEnable = VK_FALSE;
        depthStencilStateCI.depthWriteEnable = VK_FALSE;
        depthStencilStateCI.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencilStateCI.front = depthStencilStateCI.back;
        depthStencilStateCI.back.compareOp = VK_COMPARE_OP_ALWAYS;

        VkPipelineViewportStateCreateInfo viewportStateCI{};
        viewportStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCI.viewportCount = 1;
        viewportStateCI.scissorCount = 1;

        VkPipelineMultisampleStateCreateInfo multisampleStateCI{};
        multisampleStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicStateCI{};
        dynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCI.pDynamicStates = dynamicStateEnables.data();
        dynamicStateCI.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

        // Vertex input state
        VkVertexInputBindingDescription vertexInputBinding = { 0, sizeof(MeshVertex), VK_VERTEX_INPUT_RATE_VERTEX };
        VkVertexInputAttributeDescription vertexInputAttribute = { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 };

        VkPipelineVertexInputStateCreateInfo vertexInputStateCI{};
        vertexInputStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCI.vertexBindingDescriptionCount = 1;
        vertexInputStateCI.pVertexBindingDescriptions = &vertexInputBinding;
        vertexInputStateCI.vertexAttributeDescriptionCount = 1;
        vertexInputStateCI.pVertexAttributeDescriptions = &vertexInputAttribute;

        Ref<VulkanShader> shader = CreateRef<VulkanShader>("Skybox Shader");
        ShaderSourceList list;
        list.emplace_back(shaderc_glsl_vertex_shader, "filtercube.vert");
        switch (target) {
        case IRRADIANCE:
            list.emplace_back(shaderc_glsl_fragment_shader, "irradiancecube.frag");
            break;
        case PREFILTEREDENV:
            list.emplace_back(shaderc_glsl_fragment_shader, "prefilterenvmap.frag");
            break;
        };
        shader->SetDevice(m_Device);
        shader->SetIsBinary(true);
        shader->Initialize(list);

        VkGraphicsPipelineCreateInfo pipelineCI{};
        pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCI.layout = pipelinelayout;
        pipelineCI.renderPass = renderpass;
        pipelineCI.pInputAssemblyState = &inputAssemblyStateCI;
        pipelineCI.pVertexInputState = &vertexInputStateCI;
        pipelineCI.pRasterizationState = &rasterizationStateCI;
        pipelineCI.pColorBlendState = &colorBlendStateCI;
        pipelineCI.pMultisampleState = &multisampleStateCI;
        pipelineCI.pViewportState = &viewportStateCI;
        pipelineCI.pDepthStencilState = &depthStencilStateCI;
        pipelineCI.pDynamicState = &dynamicStateCI;
        pipelineCI.stageCount = 2;
        pipelineCI.renderPass = renderpass;
        pipelineCI.stageCount = shader->GetShaderInfo().size();
        pipelineCI.pStages = shader->GetShaderInfo().data();

        VkPipeline pipeline;
        VK_CHECK(vkCreateGraphicsPipelines(m_Device, m_PipelineCache, 1, &pipelineCI, nullptr, &pipeline));

        shader->Finalize();

        // Render cubemap
        VkClearValue clearValues[1];
        clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 0.0f } };

        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = renderpass;
        renderPassBeginInfo.framebuffer = offscreen.framebuffer;
        renderPassBeginInfo.renderArea.extent.width = dim;
        renderPassBeginInfo.renderArea.extent.height = dim;
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = clearValues;

        std::vector<glm::mat4> matrices = {
            glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
            glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
            glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
            glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
            glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
            glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        };

        VkCommandBuffer cmdBuf = m_LogicalDevice->CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, false);

        VkViewport viewport{};
        viewport.width = (float)dim;
        viewport.height = (float)dim;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.extent.width = dim;
        scissor.extent.height = dim;

        VkImageSubresourceRange subresourceRange{};
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = numMips;
        subresourceRange.layerCount = 6;

        // Change image layout for all cubemap faces to transfer destination
        {
            m_LogicalDevice->BeginCommandBuffer(cmdBuf);
            VkImageMemoryBarrier imageMemoryBarrier{};
            imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imageMemoryBarrier.image = cubemap.image;
            imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            imageMemoryBarrier.srcAccessMask = 0;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.subresourceRange = subresourceRange;
            vkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
            m_LogicalDevice->FlushCommandBuffer(cmdBuf, m_GraphicsQueue, false);
        }

        for (uint32_t m = 0; m < numMips; m++)
        {
            for (uint32_t f = 0; f < 6; f++)
            {
                m_LogicalDevice->BeginCommandBuffer(cmdBuf);

                viewport.width = static_cast<float>(dim * std::pow(0.5f, m));
                viewport.height = static_cast<float>(dim * std::pow(0.5f, m));
                vkCmdSetViewport(cmdBuf, 0, 1, &viewport);
                vkCmdSetScissor(cmdBuf, 0, 1, &scissor);

                // Render scene from cube face's point of view
                vkCmdBeginRenderPass(cmdBuf, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

                // Pass parameters for current pass using a push constant block
                switch (target) {
                case IRRADIANCE:
                    pushBlockIrradiance.mvp = glm::perspective((float)(M_PI / 2.0), 1.0f, 0.1f, 512.0f) * matrices[f];
                    vkCmdPushConstants(cmdBuf, pipelinelayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushBlockIrradiance), &pushBlockIrradiance);
                    break;
                case PREFILTEREDENV:
                    pushBlockPrefilterEnv.mvp = glm::perspective((float)(M_PI / 2.0), 1.0f, 0.1f, 512.0f) * matrices[f];
                    pushBlockPrefilterEnv.roughness = (float)m / (float)(numMips - 1);
                    vkCmdPushConstants(cmdBuf, pipelinelayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushBlockPrefilterEnv), &pushBlockPrefilterEnv);
                    break;
                };

                vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
                vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelinelayout, 0, 1, &descriptorset, 0, NULL);

                VkDeviceSize offsets[1] = { 0 };

                // TODO : Finish Draw Skybox Function
                // Draw Skybox
                {
                    //models.skybox.draw(cmdBuf);
                    //const VkDeviceSize offsets[1] = { 0 };
                    //vkCmdBindVertexBuffers(cmdBuf, 0, 1, &vertices.buffer, offsets);
                    //vkCmdBindIndexBuffer(cmdBuf, indices.buffer, 0, VK_INDEX_TYPE_UINT32);
                    //for (auto& node : nodes) {
                    //    drawNode(node, cmdBuf);
                    //}
                }

                vkCmdEndRenderPass(cmdBuf);

                VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
                subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                subresourceRange.baseMipLevel = 0;
                subresourceRange.levelCount = numMips;
                subresourceRange.layerCount = 6;

                {
                    VkImageMemoryBarrier imageMemoryBarrier{};
                    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    imageMemoryBarrier.image = offscreen.image;
                    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
                    vkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
                }

                // Copy region for transfer from framebuffer to cube face
                VkImageCopy copyRegion{};

                copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                copyRegion.srcSubresource.baseArrayLayer = 0;
                copyRegion.srcSubresource.mipLevel = 0;
                copyRegion.srcSubresource.layerCount = 1;
                copyRegion.srcOffset = { 0, 0, 0 };

                copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                copyRegion.dstSubresource.baseArrayLayer = f;
                copyRegion.dstSubresource.mipLevel = m;
                copyRegion.dstSubresource.layerCount = 1;
                copyRegion.dstOffset = { 0, 0, 0 };

                copyRegion.extent.width = static_cast<uint32_t>(viewport.width);
                copyRegion.extent.height = static_cast<uint32_t>(viewport.height);
                copyRegion.extent.depth = 1;

                vkCmdCopyImage(
                    cmdBuf,
                    offscreen.image,
                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    cubemap.image,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1,
                    &copyRegion);

                {
                    VkImageMemoryBarrier imageMemoryBarrier{};
                    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    imageMemoryBarrier.image = offscreen.image;
                    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
                    vkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
                }

                m_LogicalDevice->FlushCommandBuffer(cmdBuf, m_GraphicsQueue, false);
            }
        }

        {
            m_LogicalDevice->BeginCommandBuffer(cmdBuf);
            VkImageMemoryBarrier imageMemoryBarrier{};
            imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imageMemoryBarrier.image = cubemap.image;
            imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.subresourceRange = subresourceRange;
            vkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
            m_LogicalDevice->FlushCommandBuffer(cmdBuf, m_GraphicsQueue, false);
        }

        vkDestroyRenderPass(m_Device, renderpass, nullptr);
        vkDestroyFramebuffer(m_Device, offscreen.framebuffer, nullptr);
        vkFreeMemory(m_Device, offscreen.memory, nullptr);
        vkDestroyImageView(m_Device, offscreen.view, nullptr);
        vkDestroyImage(m_Device, offscreen.image, nullptr);
        vkDestroyDescriptorPool(m_Device, descriptorpool, nullptr);
        vkDestroyDescriptorSetLayout(m_Device, descriptorsetlayout, nullptr);
        vkDestroyPipeline(m_Device, pipeline, nullptr);
        vkDestroyPipelineLayout(m_Device, pipelinelayout, nullptr);

        cubemap.descriptor.imageView = cubemap.view;
        cubemap.descriptor.sampler = cubemap.sampler;
        cubemap.descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        cubemap.device = m_LogicalDevice;

        if (!m_IrradianceCube)
        {
            m_IrradianceCube = CreateRef<TextureCubeMap>();
        }
        if (!m_PrefilteredCube)
        {
            m_PrefilteredCube = CreateRef<TextureCubeMap>();
        }

        switch (target) {
        case IRRADIANCE:
            *m_IrradianceCube = cubemap;
            break;
        case PREFILTEREDENV:
            *m_PrefilteredCube = cubemap;
            //shaderValuesParams.prefilteredCubeMipLevels = static_cast<float>(numMips);
            break;
        };

        auto tEnd = std::chrono::high_resolution_clock::now();
        auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
        RK_GRAPHICS_INFO("Generating cube map with {} mip levels took {} ms", numMips, tDiff);
    }
}

void VulkanGraphicsManager::GenerateBRDFLUT()
{
    int32_t dim = 512;

    auto tStart = std::chrono::high_resolution_clock::now();

    const VkFormat format = VK_FORMAT_R16G16_SFLOAT;

    // Image
    VkImageCreateInfo imageCI{};
    imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCI.imageType = VK_IMAGE_TYPE_2D;
    imageCI.format = format;
    imageCI.extent.width = dim;
    imageCI.extent.height = dim;
    imageCI.extent.depth = 1;
    imageCI.mipLevels = 1;
    imageCI.arrayLayers = 1;
    imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCI.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    VK_CHECK(vkCreateImage(m_Device, &imageCI, nullptr, &m_BRDFImage));
    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(m_Device, m_BRDFImage, &memReqs);
    VkMemoryAllocateInfo memAllocInfo{};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.allocationSize = memReqs.size;

    VkPhysicalDeviceMemoryProperties memRequirements;
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memRequirements);

    memAllocInfo.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memRequirements);
    VK_CHECK(vkAllocateMemory(m_Device, &memAllocInfo, nullptr, &m_BRDFImageMemory));
    VK_CHECK(vkBindImageMemory(m_Device, m_BRDFImage, m_BRDFImageMemory, 0));

    // View
    VkImageViewCreateInfo viewCI{};
    viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewCI.format = format;
    viewCI.subresourceRange = {};
    viewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewCI.subresourceRange.levelCount = 1;
    viewCI.subresourceRange.layerCount = 1;
    viewCI.image = m_BRDFImage;
    VK_CHECK(vkCreateImageView(m_Device, &viewCI, nullptr, &m_BRDFImageView));

    // Sampler
    VkSamplerCreateInfo samplerCI{};
    samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCI.magFilter = VK_FILTER_LINEAR;
    samplerCI.minFilter = VK_FILTER_LINEAR;
    samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCI.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCI.minLod = 0.0f;
    samplerCI.maxLod = 1.0f;
    samplerCI.maxAnisotropy = 1.0f;
    samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    VK_CHECK(vkCreateSampler(m_Device, &samplerCI, nullptr, &m_BRDFSampler));

    // FB, Att, RP, Pipe, etc.
    VkAttachmentDescription attDesc{};
    // Color attachment
    attDesc.format = format;
    attDesc.samples = VK_SAMPLE_COUNT_1_BIT;
    attDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    VkAttachmentReference colorReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;

    // Use subpass dependencies for layout transitions
    std::array<VkSubpassDependency, 2> dependencies;
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // Create the actual renderpass
    VkRenderPassCreateInfo renderPassCI{};
    renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCI.attachmentCount = 1;
    renderPassCI.pAttachments = &attDesc;
    renderPassCI.subpassCount = 1;
    renderPassCI.pSubpasses = &subpassDescription;
    renderPassCI.dependencyCount = 2;
    renderPassCI.pDependencies = dependencies.data();

    VkRenderPass renderpass;
    VK_CHECK(vkCreateRenderPass(m_Device, &renderPassCI, nullptr, &renderpass));

    VkFramebufferCreateInfo framebufferCI{};
    framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCI.renderPass = renderpass;
    framebufferCI.attachmentCount = 1;
    framebufferCI.pAttachments = &m_BRDFImageView;
    framebufferCI.width = dim;
    framebufferCI.height = dim;
    framebufferCI.layers = 1;

    VkFramebuffer framebuffer;
    VK_CHECK(vkCreateFramebuffer(m_Device, &framebufferCI, nullptr, &framebuffer));

    // Desriptors
    VkDescriptorSetLayout descriptorsetlayout;
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCI{};
    descriptorSetLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    VK_CHECK(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCI, nullptr, &descriptorsetlayout));

    // Pipeline layout
    VkPipelineLayout pipelinelayout;
    VkPipelineLayoutCreateInfo pipelineLayoutCI{};
    pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCI.setLayoutCount = 1;
    pipelineLayoutCI.pSetLayouts = &descriptorsetlayout;
    VK_CHECK(vkCreatePipelineLayout(m_Device, &pipelineLayoutCI, nullptr, &pipelinelayout));

    // Pipeline
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCI{};
    inputAssemblyStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCI.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCI{};
    rasterizationStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCI.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCI.cullMode = VK_CULL_MODE_NONE;
    rasterizationStateCI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCI.lineWidth = 1.0f;

    VkPipelineColorBlendAttachmentState blendAttachmentState{};
    blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    blendAttachmentState.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlendStateCI{};
    colorBlendStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCI.attachmentCount = 1;
    colorBlendStateCI.pAttachments = &blendAttachmentState;

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCI{};
    depthStencilStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCI.depthTestEnable = VK_FALSE;
    depthStencilStateCI.depthWriteEnable = VK_FALSE;
    depthStencilStateCI.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilStateCI.front = depthStencilStateCI.back;
    depthStencilStateCI.back.compareOp = VK_COMPARE_OP_ALWAYS;

    VkPipelineViewportStateCreateInfo viewportStateCI{};
    viewportStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCI.viewportCount = 1;
    viewportStateCI.scissorCount = 1;

    VkPipelineMultisampleStateCreateInfo multisampleStateCI{};
    multisampleStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicStateCI{};
    dynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCI.pDynamicStates = dynamicStateEnables.data();
    dynamicStateCI.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
    
    VkPipelineVertexInputStateCreateInfo emptyInputStateCI{};
    emptyInputStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    // Look-up-table (from BRDF) pipeline
    ShaderSourceList list;
    list.emplace_back(shaderc_glsl_vertex_shader, "genbrdflut.vert");
    list.emplace_back(shaderc_glsl_fragment_shader, "genbrdflut.frag");
    Ref<VulkanShader> shader = CreateRef<VulkanShader>("BRDF Shader");
    shader->SetDevice(m_Device);
    shader->SetIsBinary(true);
    shader->Initialize(list);

    VkGraphicsPipelineCreateInfo pipelineCI{};
    pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCI.layout = pipelinelayout;
    pipelineCI.renderPass = renderpass;
    pipelineCI.pInputAssemblyState = &inputAssemblyStateCI;
    pipelineCI.pVertexInputState = &emptyInputStateCI;
    pipelineCI.pRasterizationState = &rasterizationStateCI;
    pipelineCI.pColorBlendState = &colorBlendStateCI;
    pipelineCI.pMultisampleState = &multisampleStateCI;
    pipelineCI.pViewportState = &viewportStateCI;
    pipelineCI.pDepthStencilState = &depthStencilStateCI;
    pipelineCI.pDynamicState = &dynamicStateCI;
    pipelineCI.stageCount = 2;
    pipelineCI.pStages = shader->GetShaderInfo().data();//shaderStages.data();

    VkPipeline pipeline;
    VK_CHECK(vkCreateGraphicsPipelines(m_Device, m_PipelineCache, 1, &pipelineCI, nullptr, &pipeline));
    shader->Finalize();

    // Render
    VkClearValue clearValues[1];
    clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderpass;
    renderPassBeginInfo.renderArea.extent.width = dim;
    renderPassBeginInfo.renderArea.extent.height = dim;
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = clearValues;
    renderPassBeginInfo.framebuffer = framebuffer;

    VkCommandBufferAllocateInfo cmdBufAllocateInfo{};
    cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufAllocateInfo.commandPool = m_CommandPool;
    cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufAllocateInfo.commandBufferCount = 1;

    VkCommandBuffer cmdBuffer;
    VK_CHECK(vkAllocateCommandBuffers(m_Device, &cmdBufAllocateInfo, &cmdBuffer));

    // If requested, also start recording for the new command buffer
    VkCommandBufferBeginInfo commandBufferBI{};
    commandBufferBI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VK_CHECK(vkBeginCommandBuffer(cmdBuffer, &commandBufferBI));

    VkCommandBuffer cmdBuf = cmdBuffer;
    vkCmdBeginRenderPass(cmdBuf, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.width = (float)dim;
    viewport.height = (float)dim;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.extent.width = dim;
    scissor.extent.height = dim;

    vkCmdSetViewport(cmdBuf, 0, 1, &viewport);
    vkCmdSetScissor(cmdBuf, 0, 1, &scissor);
    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdDraw(cmdBuf, 3, 1, 0, 0);
    vkCmdEndRenderPass(cmdBuf);

    //VkQueue queue;
    //vkGetDeviceQueue(m_Device, VK_QUEUE_GRAPHICS_BIT, 0, &queue);

    VK_CHECK(vkEndCommandBuffer(cmdBuf));

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuf;

    // Create fence to ensure that the command buffer has finished executing
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence fence;
    VK_CHECK(vkCreateFence(m_Device, &fenceInfo, nullptr, &fence));
    
    // Submit to the queue
    VK_CHECK(vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, fence));
    // Wait for the fence to signal that command buffer has finished executing
    VK_CHECK(vkWaitForFences(m_Device, 1, &fence, VK_TRUE, 100000000000));

    vkDestroyFence(m_Device, fence, nullptr);

    vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &cmdBuf);

    vkQueueWaitIdle(m_GraphicsQueue);

    vkDestroyPipeline(m_Device, pipeline, nullptr);
    vkDestroyPipelineLayout(m_Device, pipelinelayout, nullptr);
    vkDestroyRenderPass(m_Device, renderpass, nullptr);
    vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
    vkDestroyDescriptorSetLayout(m_Device, descriptorsetlayout, nullptr);

    //textures.lutBrdf.descriptor.imageView = textures.lutBrdf.view;
    //textures.lutBrdf.descriptor.sampler = textures.lutBrdf.sampler;
    //textures.lutBrdf.descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    //textures.lutBrdf.device = vulkanDevice;

    auto tEnd = std::chrono::high_resolution_clock::now();
    auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
    RK_GRAPHICS_INFO("Generating BRDF LUT took {} ms", tDiff);
}

void VulkanGraphicsManager::DrawFullScreenQuad()
{
}

bool VulkanGraphicsManager::OnWindowResize(EventPtr& e)
{
    m_FramebufferResized = true;
    return false;
}

//--------------------------------------------------------------------//
//--- Debug ----------------------------------------------------------//
//--------------------------------------------------------------------//

void VulkanGraphicsManager::DrawPoint(const Point3D& point, const Vector3f& color)
{
}

void VulkanGraphicsManager::DrawPointSet(const Point3DSet& point_set, const Vector3f& color)
{
}

void VulkanGraphicsManager::DrawPointSet(const Point3DSet& point_set, const Matrix4f& trans, const Vector3f& color)
{
}

void VulkanGraphicsManager::DrawLine(const Point3D& from, const Point3D& to, const Vector3f& color)
{
}

void VulkanGraphicsManager::DrawLine(const Point3DList& vertices, const Vector3f& color)
{
}

void VulkanGraphicsManager::DrawLine(const Point3DList& vertices, const Matrix4f& trans, const Vector3f& color)
{
}

void VulkanGraphicsManager::DrawTriangle(const Point3DList& vertices, const Vector3f& color)
{
}

void VulkanGraphicsManager::DrawTriangle(const Point3DList& vertices, const Matrix4f& trans, const Vector3f& color)
{
}

void VulkanGraphicsManager::DrawTriangleStrip(const Point3DList& vertices, const Vector3f& color)
{
}
