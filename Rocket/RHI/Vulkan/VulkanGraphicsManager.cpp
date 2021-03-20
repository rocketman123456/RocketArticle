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
    bool ret = GraphicsManager::Initialize();
    if (ret)
    {
        RK_GRAPHICS_ERROR("GraphicsManager Initialize Failed");
        return ret;
    }

    // Get Config
    auto config = g_Application->GetConfig();
    // Get Window Handle
    window_handle_ = static_cast<GLFWwindow*>(g_WindowManager->GetNativeWindow());
    // Must be : 1, 2, 4, 8, 16, 32, 64
    msaa_samples_ = (VkSampleCountFlagBits)config->GetConfigInfo<uint32_t>("Graphics", "msaa_sample_count");

    CreateInstance();
    SetupDebugMessenger();
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();

    CreateSwapChain();
    CreateGraphicsPipeline();
    CreateFramebuffers();
    CreateSyncObjects();
    CreateUniformBuffers();
    
    InitGui();

    return 0;
}

void VulkanGraphicsManager::Finalize()
{
    vkDeviceWaitIdle(device_);

    GraphicsManager::Finalize();

    CleanupSwapChain();

    ui_->Finalize();
    vulkan_swapchain_->Finalize();
    vulkan_pipeline_->Finalize();

    vkDestroyPipelineCache(device_, pipeline_cache_, nullptr);

    // Destroy Uniform Buffers
    for (size_t i = 0; i < swapchain_images_.size(); i++)
    {
        vkDestroyBuffer(device_, uniform_buffers_[i], nullptr);
        vkFreeMemory(device_, uniform_buffers_memory_[i], nullptr);
    }

    // Destroy SyncObjects
    for (size_t i = 0; i < max_frame_in_flight_; i++)
    {
        vkDestroySemaphore(device_, render_finished_semaphores_[i], nullptr);
        vkDestroySemaphore(device_, image_available_semaphores_[i], nullptr);
        vkDestroyFence(device_, in_flight_fences_[i], nullptr);
    }

    logical_device_->Finalize();

    if (enableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
    }

    vkDestroySurfaceKHR(instance_, surface_, nullptr);
    vkDestroyInstance(instance_, nullptr);
}

void VulkanGraphicsManager::CleanupSwapChain()
{
    vkFreeCommandBuffers(device_, command_pool_, static_cast<uint32_t>(command_buffers_.size()), command_buffers_.data());
    vkFreeCommandBuffers(device_, command_pool_, static_cast<uint32_t>(gui_command_buffer_.size()), gui_command_buffer_.data());

    vulkan_framebuffer_->Finalize();
    //m_VulkanPipeline->Finalize();
    //m_VulkanSwapChain->Finalize();
    //ui_->Finalize();

    RK_GRAPHICS_TRACE("CleanupSwapChain");
}

void VulkanGraphicsManager::RecreateSwapChain()
{
    RK_GRAPHICS_TRACE("RecreateSwapChain");
    is_scene_prepared_ = false;

    int width = 0, height = 0;
    glfwGetFramebufferSize(window_handle_, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(window_handle_, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device_);

    CleanupSwapChain();

    CreateSwapChain();

    //InitGui();
    CreateFramebuffers();
    CreateCommandBuffers();

    for (uint32_t i = 0; i < command_buffers_.size(); ++i)
    {
        RecordCommandBuffer(i);
    }
    
    vkDeviceWaitIdle(device_);

    images_in_flight_.resize(swapchain_images_.size(), VK_NULL_HANDLE);

    is_scene_prepared_ = true;
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

    if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS)
        RK_GRAPHICS_ERROR("failed to create instance!");
}

void VulkanGraphicsManager::SetupDebugMessenger()
{
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);

    VK_CHECK(CreateDebugUtilsMessengerEXT(instance_, &createInfo, nullptr, &debug_messenger_));
}

void VulkanGraphicsManager::CreateSurface()
{
    VK_CHECK(glfwCreateWindowSurface(instance_, window_handle_, nullptr, &surface_));
}

void VulkanGraphicsManager::PickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);

    if (deviceCount == 0)
        RK_GRAPHICS_ERROR("failed to find GPUs with Vulkan support!");

    Vec<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        if (IsDeviceSuitable(device, surface_))
        {
            physical_device_ = device;
            // TODO : use config file
            msaa_samples_ = GetMaxUsableSampleCount(physical_device_);
            //m_MsaaSamples = VK_SAMPLE_COUNT_4_BIT;
            break;
        }
    }

    if (physical_device_ == VK_NULL_HANDLE)
        RK_GRAPHICS_ERROR("failed to find a suitable GPU!");

    vkGetPhysicalDeviceProperties(physical_device_, &device_properties_);
    vkGetPhysicalDeviceFeatures(physical_device_, &device_features_);
    vkGetPhysicalDeviceMemoryProperties(physical_device_, &device_memory_properties_);
}

void VulkanGraphicsManager::CreateLogicalDevice()
{
    if (!logical_device_)
    {
        logical_device_ = CreateRef<VulkanDevice>(physical_device_, surface_);
    }
    logical_device_->Initialize();

    VkPhysicalDeviceFeatures enabledFeatures{};
    if (device_features_.samplerAnisotropy) {
        enabledFeatures.samplerAnisotropy = VK_TRUE;
    }
    Vec<const char*> enabledExtensions{};
    VK_CHECK(logical_device_->CreateLogicalDevice(enabledFeatures, enabledExtensions, enableValidationLayers, validationLayers));

    device_ = logical_device_->logical_device;
    command_pool_ = logical_device_->command_pool;
    graphics_queue_ = logical_device_->graphics_queue;
    compute_queue_ = logical_device_->compute_queue;
    present_queue_ = logical_device_->present_queue;
}

void VulkanGraphicsManager::CreateSwapChain()
{
    if (!vulkan_swapchain_)
    {
        vulkan_swapchain_ = CreateRef<VulkanSwapchain>();
        vulkan_swapchain_->SetWindowHandle(window_handle_);
    }
    vulkan_swapchain_->Connect(instance_, physical_device_, device_, surface_);
    vulkan_swapchain_->Initialize(true);
    
    swap_chain_ = vulkan_swapchain_->swapchain;
    swapchain_images_.clear();
    swapchain_image_views_.clear();
    swapchain_image_format_ = vulkan_swapchain_->color_format;
    swapchain_extent_ = vulkan_swapchain_->extent;
    for (auto buffer : vulkan_swapchain_->buffers)
    {
        swapchain_images_.push_back(buffer.image);
        swapchain_image_views_.push_back(buffer.view);
    }
}

void VulkanGraphicsManager::CreateGraphicsPipeline()
{
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
    pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    VK_CHECK(vkCreatePipelineCache(device_, &pipelineCacheCreateInfo, nullptr, &pipeline_cache_));

    if (!vulkan_pipeline_)
    {
        vulkan_pipeline_ = CreateRef<VulkanPipeline>();
        vulkan_pipeline_->SetWindowHandle(window_handle_);
        vulkan_pipeline_->SetMsaaSample(msaa_samples_);
    }
    vulkan_pipeline_->Connect(instance_, physical_device_, device_, surface_);
    vulkan_pipeline_->Initialize();

    render_pass_ = vulkan_pipeline_->render_pass;
    gui_render_pass_ = vulkan_pipeline_->gui_render_pass;
    descriptor_set_layout_ = vulkan_pipeline_->descriptor_set_layout;
    pipeline_layout_ = vulkan_pipeline_->pipeline_layout;
    graphics_pipeline_ = vulkan_pipeline_->graphics_pipeline;
}

void VulkanGraphicsManager::CreateFramebuffers()
{
    if (!vulkan_framebuffer_)
    {
        vulkan_framebuffer_ = CreateRef<VulkanFrameBuffer>();
        vulkan_framebuffer_->SetWindowHandle(window_handle_);
        vulkan_framebuffer_->SetMsaaSample(msaa_samples_);
    }
    vulkan_framebuffer_->Connect(device_, physical_device_, surface_, render_pass_, swapchain_image_views_);
    vulkan_framebuffer_->Initialize();

    color_image_ = vulkan_framebuffer_->color_image;
    color_image_memory_ = vulkan_framebuffer_->color_image_memory;
    color_image_view_ = vulkan_framebuffer_->color_image_view;

    depth_image_ = vulkan_framebuffer_->depth_image;
    depth_image_memory_ = vulkan_framebuffer_->depth_image_memory;
    depth_image_view_ = vulkan_framebuffer_->depth_image_view;

    swapchain_framebuffers_.resize(vulkan_framebuffer_->swapchain_framebuffers.size());
    for (size_t i = 0; i < swapchain_framebuffers_.size(); ++i)
    {
        swapchain_framebuffers_[i] = vulkan_framebuffer_->swapchain_framebuffers[i];
    }
}

void VulkanGraphicsManager::CreateSyncObjects()
{
    image_available_semaphores_.resize(max_frame_in_flight_);
    render_finished_semaphores_.resize(max_frame_in_flight_);
    in_flight_fences_.resize(max_frame_in_flight_);
    images_in_flight_.resize(swapchain_images_.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < max_frame_in_flight_; i++)
    {
        VK_CHECK(vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &image_available_semaphores_[i]));
        VK_CHECK(vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &render_finished_semaphores_[i]));
        VK_CHECK(vkCreateFence(device_, &fenceInfo, nullptr, &in_flight_fences_[i]));
    }
}

void VulkanGraphicsManager::InitGui()
{
    if (!ui_)
    {
        ui_ = CreateRef<VulkanUI>();
        auto vulkanUI = static_cast<VulkanUI*>(ui_.get());
        vulkanUI->SetWindowHandle(window_handle_);
    }
    auto vulkanUI = static_cast<VulkanUI*>(ui_.get());
    vulkanUI->Connect(instance_, logical_device_, render_pass_, graphics_queue_, pipeline_cache_, msaa_samples_, max_frame_in_flight_);
    ui_->Initialize();

    ui_->UpdataOverlay(swapchain_extent_.width, swapchain_extent_.height);
}

void VulkanGraphicsManager::CreateCommandBuffers()
{
    command_buffers_.resize(swapchain_framebuffers_.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = command_pool_;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(command_buffers_.size());

    VK_CHECK(vkAllocateCommandBuffers(device_, &allocInfo, command_buffers_.data()));
    
    gui_command_buffer_.resize(swapchain_framebuffers_.size());

    VkCommandBufferAllocateInfo allocInfoGui{};
    allocInfoGui.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfoGui.commandPool = command_pool_;
    allocInfoGui.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfoGui.commandBufferCount = static_cast<uint32_t>(gui_command_buffer_.size());

    VK_CHECK(vkAllocateCommandBuffers(device_, &allocInfoGui, gui_command_buffer_.data()));
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
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices_.size());
                vertices_.push_back(vertex);
            }

            indices_.push_back(uniqueVertices[vertex]);
        }
    }
}

void VulkanGraphicsManager::CreateVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(vertices_[0]) * vertices_.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(device_, physical_device_, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device_, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices_.data(), (size_t)bufferSize);
    vkUnmapMemory(device_, stagingBufferMemory);

    CreateBuffer(device_, physical_device_, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertex_buffer_, vertex_buffer_memory_);

    CopyBuffer(device_, command_pool_, graphics_queue_, stagingBuffer, vertex_buffer_, bufferSize);

    vkDestroyBuffer(device_, stagingBuffer, nullptr);
    vkFreeMemory(device_, stagingBufferMemory, nullptr);
}

void VulkanGraphicsManager::CreateIndexBuffer()
{
    VkDeviceSize bufferSize = sizeof(indices_[0]) * indices_.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(device_, physical_device_, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device_, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices_.data(), (size_t)bufferSize);
    vkUnmapMemory(device_, stagingBufferMemory);

    CreateBuffer(device_, physical_device_, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, index_buffer_, index_buffer_memory_);

    CopyBuffer(device_, command_pool_, graphics_queue_, stagingBuffer, index_buffer_, bufferSize);

    vkDestroyBuffer(device_, stagingBuffer, nullptr);
    vkFreeMemory(device_, stagingBufferMemory, nullptr);
}

void VulkanGraphicsManager::CreateTextureImage()
{
    vulkan_texture_2d_ = CreateRef<VulkanTexture2D>();
    vulkan_texture_2d_->LoadFromFile("Models/viking_room.png", VK_FORMAT_R8G8B8A8_SRGB, logical_device_, graphics_queue_);

    mip_levels_ = vulkan_texture_2d_->mip_levels;
    texture_image_ = vulkan_texture_2d_->image;
    texture_image_memory_ = vulkan_texture_2d_->device_memory;
    texture_image_view_ = vulkan_texture_2d_->view;
    texture_sampler_ = vulkan_texture_2d_->sampler;

    if (environment_cube_) {
        environment_cube_->Finalize();
        irradiance_cube_->Finalize();
        prefiltered_cube_->Finalize();
    }
    String filename = "Textures/environments/papermill.ktx";
    RK_GRAPHICS_INFO("Loading environment from {}", filename);
    environment_cube_ = CreateRef<TextureCubeMap>();
    environment_cube_->LoadFromFile(filename, VK_FORMAT_R16G16B16A16_SFLOAT, logical_device_, graphics_queue_);
    GenerateCubeMaps();
}

void VulkanGraphicsManager::CreateUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniform_buffers_.resize(swapchain_images_.size());
    uniform_buffers_memory_.resize(swapchain_images_.size());

    for (size_t i = 0; i < swapchain_images_.size(); i++)
    {
        CreateBuffer(
            device_,
            physical_device_,
            bufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            uniform_buffers_[i],
            uniform_buffers_memory_[i]
        );
    }
}

void VulkanGraphicsManager::CreateDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(swapchain_images_.size());
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(swapchain_images_.size());

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(swapchain_images_.size());

    VK_CHECK(vkCreateDescriptorPool(device_, &poolInfo, nullptr, &descriptor_pool_));
}

void VulkanGraphicsManager::CreateDescriptorSets()
{
    Vec<VkDescriptorSetLayout> layouts(swapchain_images_.size(), descriptor_set_layout_);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptor_pool_;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapchain_images_.size());
    allocInfo.pSetLayouts = layouts.data();

    descriptor_sets_.resize(swapchain_images_.size());
    VK_CHECK(vkAllocateDescriptorSets(device_, &allocInfo, descriptor_sets_.data()));

    for (size_t i = 0; i < swapchain_images_.size(); i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniform_buffers_[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = texture_image_view_;
        imageInfo.sampler = texture_sampler_;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptor_sets_[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptor_sets_[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(device_, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
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

    //InitGui();

    for (uint32_t i = 0; i < command_buffers_.size(); ++i)
    {
        RecordCommandBuffer(i);
    }

    is_scene_prepared_ = true;
}

void VulkanGraphicsManager::EndScene()
{
    GraphicsManager::EndScene();

    if (is_scene_prepared_)
    {
        //vkFreeCommandBuffers(m_Device, m_CommandPool, static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
        //m_CommandBuffers.clear();
        //m_CommandBuffers.resize(max_frame_in_flight_);

        //ui_->Finalize();

        // Clear DescriptorPool / DescriptorSet
        vkDestroyDescriptorPool(device_, descriptor_pool_, nullptr);

        // Clear BRDF
        vkDestroySampler(device_, brdf_sampler_, nullptr);
        vkDestroyImageView(device_, brdf_image_view_, nullptr);
        vkDestroyImage(device_, brdf_image_, nullptr);
        vkFreeMemory(device_, brdf_image_memory_, nullptr);

        // Clear Model Data
        vulkan_texture_2d_->Finalize();
        vulkan_texture_2d_.reset();

        environment_cube_->Finalize();
        irradiance_cube_->Finalize();
        prefiltered_cube_->Finalize();
        environment_cube_.reset();
        irradiance_cube_.reset();
        prefiltered_cube_.reset();

        vkDestroyBuffer(device_, index_buffer_, nullptr);
        vkFreeMemory(device_, index_buffer_memory_, nullptr);

        vkDestroyBuffer(device_, vertex_buffer_, nullptr);
        vkFreeMemory(device_, vertex_buffer_memory_, nullptr);
    }
    is_scene_prepared_ = false;
}

void VulkanGraphicsManager::UpdateUniformBuffer(uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swapchain_extent_.width / (float)swapchain_extent_.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    void* data;
    vkMapMemory(device_, uniform_buffers_memory_[currentImage], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device_, uniform_buffers_memory_[currentImage]);
}

void VulkanGraphicsManager::SetPipelineState(const Ref<PipelineState>& pipelineState, const Frame& frame)
{
}

void VulkanGraphicsManager::RecordCommandBuffer(uint32_t frame_index)
{
    // Record Command Buffer
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VK_CHECK(vkBeginCommandBuffer(command_buffers_[frame_index], &beginInfo));

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = render_pass_;
    renderPassInfo.framebuffer = swapchain_framebuffers_[frame_index];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapchain_extent_;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(command_buffers_[frame_index], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    {
        VkViewport viewport{};
        viewport.width = (float)swapchain_extent_.width;
        viewport.height = (float)swapchain_extent_.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(command_buffers_[frame_index], 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.extent = { (uint32_t)swapchain_extent_.width, (uint32_t)swapchain_extent_.height };
        vkCmdSetScissor(command_buffers_[frame_index], 0, 1, &scissor);

        vkCmdBindPipeline(command_buffers_[frame_index], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);
        VkBuffer vertexBuffers[] = { vertex_buffer_ };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(command_buffers_[frame_index], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(command_buffers_[frame_index], index_buffer_, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(command_buffers_[frame_index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_, 0, 1, &descriptor_sets_[frame_index], 0, nullptr);
        vkCmdDrawIndexed(command_buffers_[frame_index], static_cast<uint32_t>(indices_.size()), 1, 0, 0, 0);
    }
    vkCmdEndRenderPass(command_buffers_[frame_index]);

    VK_CHECK(vkEndCommandBuffer(command_buffers_[frame_index]));
}

void VulkanGraphicsManager::RecordGuiCommandBuffer(uint32_t frame_index)
{
    // Record And Submit Gui Command
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VK_CHECK(vkBeginCommandBuffer(gui_command_buffer_[frame_index], &beginInfo));

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = gui_render_pass_;
    renderPassInfo.framebuffer = swapchain_framebuffers_[frame_index];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapchain_extent_;

    vkCmdBeginRenderPass(gui_command_buffer_[frame_index], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    {
        VkViewport viewport{};
        viewport.width = (float)swapchain_extent_.width;
        viewport.height = (float)swapchain_extent_.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(gui_command_buffer_[frame_index], 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.extent = { (uint32_t)swapchain_extent_.width, (uint32_t)swapchain_extent_.height };
        vkCmdSetScissor(gui_command_buffer_[frame_index], 0, 1, &scissor);
        // TODO : use uniform function
        auto vulkanUI = static_cast<VulkanUI*>(ui_.get());
        vulkanUI->Draw(gui_command_buffer_[frame_index]);
    }
    vkCmdEndRenderPass(gui_command_buffer_[frame_index]);

    VK_CHECK(vkEndCommandBuffer(gui_command_buffer_[frame_index]));
}

void VulkanGraphicsManager::BeginFrame(const Frame& frame)
{
    if (!is_scene_prepared_)
        return;
    
    // Reset RecreateSwapChain
    is_recreate_swapchain_ = false;

    //RK_GRAPHICS_TRACE("Begin Frame");
    //RK_CORE_TRACE("width : {}, height : {}", m_SwapChainExtent.width, m_SwapChainExtent.height);
    ui_->UpdataOverlay(swapchain_extent_.width, swapchain_extent_.height);

    vkWaitForFences(device_, 1, &in_flight_fences_[current_frame_index_], VK_TRUE, UINT64_MAX);
    VkResult acquireResult = vulkan_swapchain_->AcquireNextImage(image_available_semaphores_[current_frame_index_], &frame_index_);
    
    if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR) {
        is_recreate_swapchain_ = true;
        RecreateSwapChain();
        return;
    }
    else if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
        VK_CHECK(acquireResult);
    }

    if (images_in_flight_[frame_index_] != VK_NULL_HANDLE) {
        vkWaitForFences(device_, 1, &images_in_flight_[frame_index_], VK_TRUE, UINT64_MAX);
    }
    images_in_flight_[frame_index_] = in_flight_fences_[current_frame_index_];

    UpdateUniformBuffer(frame_index_);

    // TODO : move this to draw batch
    //RecordCommandBuffer(frame_index_);
    RecordGuiCommandBuffer(frame_index_);

    submit_buffers_.clear();
}

void VulkanGraphicsManager::EndFrame(const Frame& frame)
{
    if (!is_scene_prepared_)
        return;
    if(is_recreate_swapchain_)
    {
        //ui_->PostAction();
        return;
    }

    submit_buffers_.push_back(command_buffers_[frame_index_]);
    submit_buffers_.push_back(gui_command_buffer_[frame_index_]);

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &image_available_semaphores_[current_frame_index_];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &render_finished_semaphores_[current_frame_index_];
    submitInfo.commandBufferCount = static_cast<uint32_t>(submit_buffers_.size());
    submitInfo.pCommandBuffers = submit_buffers_.data();
    submitInfo.pWaitDstStageMask = waitStages;

    vkResetFences(device_, 1, &in_flight_fences_[current_frame_index_]);
    VK_CHECK(vkQueueSubmit(graphics_queue_, 1, &submitInfo, in_flight_fences_[current_frame_index_]));

    //ui_->PostAction();

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &render_finished_semaphores_[current_frame_index_];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swap_chain_;
    presentInfo.pImageIndices = &frame_index_;

    VkResult presentResult = vkQueuePresentKHR(present_queue_, &presentInfo);

    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR || framebuffer_resized_) {
        framebuffer_resized_ = false;
        is_recreate_swapchain_ = true;
        RecreateSwapChain();
        return;
    }
    else if (presentResult != VK_SUCCESS) {
        VK_CHECK(presentResult);
    }

    current_frame_index_ = (current_frame_index_ + 1) % max_frame_in_flight_;
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
            VK_CHECK(vkCreateImage(device_, &imageCI, nullptr, &cubemap.image));
            VkMemoryRequirements memReqs;
            vkGetImageMemoryRequirements(device_, cubemap.image, &memReqs);
            VkMemoryAllocateInfo memAllocInfo{};
            memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            memAllocInfo.allocationSize = memReqs.size;
            memAllocInfo.memoryTypeIndex = logical_device_->GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            VK_CHECK(vkAllocateMemory(device_, &memAllocInfo, nullptr, &cubemap.device_memory));
            VK_CHECK(vkBindImageMemory(device_, cubemap.image, cubemap.device_memory, 0));

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
            VK_CHECK(vkCreateImageView(device_, &viewCI, nullptr, &cubemap.view));

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
            VK_CHECK(vkCreateSampler(device_, &samplerCI, nullptr, &cubemap.sampler));
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
        VK_CHECK(vkCreateRenderPass(device_, &renderPassCI, nullptr, &renderpass));

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
            VK_CHECK(vkCreateImage(device_, &imageCI, nullptr, &offscreen.image));
            VkMemoryRequirements memReqs;
            vkGetImageMemoryRequirements(device_, offscreen.image, &memReqs);
            VkMemoryAllocateInfo memAllocInfo{};
            memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            memAllocInfo.allocationSize = memReqs.size;
            memAllocInfo.memoryTypeIndex = logical_device_->GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            VK_CHECK(vkAllocateMemory(device_, &memAllocInfo, nullptr, &offscreen.memory));
            VK_CHECK(vkBindImageMemory(device_, offscreen.image, offscreen.memory, 0));

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
            VK_CHECK(vkCreateImageView(device_, &viewCI, nullptr, &offscreen.view));

            // Framebuffer
            VkFramebufferCreateInfo framebufferCI{};
            framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCI.renderPass = renderpass;
            framebufferCI.attachmentCount = 1;
            framebufferCI.pAttachments = &offscreen.view;
            framebufferCI.width = dim;
            framebufferCI.height = dim;
            framebufferCI.layers = 1;
            VK_CHECK(vkCreateFramebuffer(device_, &framebufferCI, nullptr, &offscreen.framebuffer));

            VkCommandBuffer layoutCmd = logical_device_->CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
            VkImageMemoryBarrier imageMemoryBarrier{};
            imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imageMemoryBarrier.image = offscreen.image;
            imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            imageMemoryBarrier.srcAccessMask = 0;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
            vkCmdPipelineBarrier(layoutCmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
            logical_device_->FlushCommandBuffer(layoutCmd, graphics_queue_, true);
        }

        // Descriptors
        VkDescriptorSetLayout descriptorsetlayout;
        VkDescriptorSetLayoutBinding setLayoutBinding = { 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr };
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCI{};
        descriptorSetLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCI.pBindings = &setLayoutBinding;
        descriptorSetLayoutCI.bindingCount = 1;
        VK_CHECK(vkCreateDescriptorSetLayout(device_, &descriptorSetLayoutCI, nullptr, &descriptorsetlayout));

        // Descriptor Pool
        VkDescriptorPoolSize poolSize = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 };
        VkDescriptorPoolCreateInfo descriptorPoolCI{};
        descriptorPoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCI.poolSizeCount = 1;
        descriptorPoolCI.pPoolSizes = &poolSize;
        descriptorPoolCI.maxSets = 2;
        VkDescriptorPool descriptorpool;
        VK_CHECK(vkCreateDescriptorPool(device_, &descriptorPoolCI, nullptr, &descriptorpool));

        // Descriptor sets
        VkDescriptorSet descriptorset;
        VkDescriptorSetAllocateInfo descriptorSetAllocInfo{};
        descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocInfo.descriptorPool = descriptorpool;
        descriptorSetAllocInfo.pSetLayouts = &descriptorsetlayout;
        descriptorSetAllocInfo.descriptorSetCount = 1;
        VK_CHECK(vkAllocateDescriptorSets(device_, &descriptorSetAllocInfo, &descriptorset));
        VkWriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.dstSet = descriptorset;
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.pImageInfo = &environment_cube_->descriptor;
        vkUpdateDescriptorSets(device_, 1, &writeDescriptorSet, 0, nullptr);

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
        VK_CHECK(vkCreatePipelineLayout(device_, &pipelineLayoutCI, nullptr, &pipelinelayout));

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
        shader->SetDevice(device_);
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
        VK_CHECK(vkCreateGraphicsPipelines(device_, pipeline_cache_, 1, &pipelineCI, nullptr, &pipeline));

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

        VkCommandBuffer cmdBuf = logical_device_->CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, false);

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
            logical_device_->BeginCommandBuffer(cmdBuf);
            VkImageMemoryBarrier imageMemoryBarrier{};
            imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imageMemoryBarrier.image = cubemap.image;
            imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            imageMemoryBarrier.srcAccessMask = 0;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.subresourceRange = subresourceRange;
            vkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
            logical_device_->FlushCommandBuffer(cmdBuf, graphics_queue_, false);
        }

        for (uint32_t m = 0; m < numMips; m++)
        {
            for (uint32_t f = 0; f < 6; f++)
            {
                logical_device_->BeginCommandBuffer(cmdBuf);

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

                logical_device_->FlushCommandBuffer(cmdBuf, graphics_queue_, false);
            }
        }

        {
            logical_device_->BeginCommandBuffer(cmdBuf);
            VkImageMemoryBarrier imageMemoryBarrier{};
            imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imageMemoryBarrier.image = cubemap.image;
            imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.subresourceRange = subresourceRange;
            vkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
            logical_device_->FlushCommandBuffer(cmdBuf, graphics_queue_, false);
        }

        vkDestroyRenderPass(device_, renderpass, nullptr);
        vkDestroyFramebuffer(device_, offscreen.framebuffer, nullptr);
        vkFreeMemory(device_, offscreen.memory, nullptr);
        vkDestroyImageView(device_, offscreen.view, nullptr);
        vkDestroyImage(device_, offscreen.image, nullptr);
        vkDestroyDescriptorPool(device_, descriptorpool, nullptr);
        vkDestroyDescriptorSetLayout(device_, descriptorsetlayout, nullptr);
        vkDestroyPipeline(device_, pipeline, nullptr);
        vkDestroyPipelineLayout(device_, pipelinelayout, nullptr);

        cubemap.descriptor.imageView = cubemap.view;
        cubemap.descriptor.sampler = cubemap.sampler;
        cubemap.descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        cubemap.device = logical_device_;

        if (!irradiance_cube_)
        {
            irradiance_cube_ = CreateRef<TextureCubeMap>();
        }
        if (!prefiltered_cube_)
        {
            prefiltered_cube_ = CreateRef<TextureCubeMap>();
        }

        switch (target) {
        case IRRADIANCE:
            *irradiance_cube_ = cubemap;
            break;
        case PREFILTEREDENV:
            *prefiltered_cube_ = cubemap;
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
    VK_CHECK(vkCreateImage(device_, &imageCI, nullptr, &brdf_image_));
    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(device_, brdf_image_, &memReqs);
    VkMemoryAllocateInfo memAllocInfo{};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.allocationSize = memReqs.size;

    VkPhysicalDeviceMemoryProperties memRequirements;
    vkGetPhysicalDeviceMemoryProperties(physical_device_, &memRequirements);

    memAllocInfo.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memRequirements);
    VK_CHECK(vkAllocateMemory(device_, &memAllocInfo, nullptr, &brdf_image_memory_));
    VK_CHECK(vkBindImageMemory(device_, brdf_image_, brdf_image_memory_, 0));

    // View
    VkImageViewCreateInfo viewCI{};
    viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewCI.format = format;
    viewCI.subresourceRange = {};
    viewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewCI.subresourceRange.levelCount = 1;
    viewCI.subresourceRange.layerCount = 1;
    viewCI.image = brdf_image_;
    VK_CHECK(vkCreateImageView(device_, &viewCI, nullptr, &brdf_image_view_));

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
    VK_CHECK(vkCreateSampler(device_, &samplerCI, nullptr, &brdf_sampler_));

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
    VK_CHECK(vkCreateRenderPass(device_, &renderPassCI, nullptr, &renderpass));

    VkFramebufferCreateInfo framebufferCI{};
    framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCI.renderPass = renderpass;
    framebufferCI.attachmentCount = 1;
    framebufferCI.pAttachments = &brdf_image_view_;
    framebufferCI.width = dim;
    framebufferCI.height = dim;
    framebufferCI.layers = 1;

    VkFramebuffer framebuffer;
    VK_CHECK(vkCreateFramebuffer(device_, &framebufferCI, nullptr, &framebuffer));

    // Desriptors
    VkDescriptorSetLayout descriptorsetlayout;
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCI{};
    descriptorSetLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    VK_CHECK(vkCreateDescriptorSetLayout(device_, &descriptorSetLayoutCI, nullptr, &descriptorsetlayout));

    // Pipeline layout
    VkPipelineLayout pipelinelayout;
    VkPipelineLayoutCreateInfo pipelineLayoutCI{};
    pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCI.setLayoutCount = 1;
    pipelineLayoutCI.pSetLayouts = &descriptorsetlayout;
    VK_CHECK(vkCreatePipelineLayout(device_, &pipelineLayoutCI, nullptr, &pipelinelayout));

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
    shader->SetDevice(device_);
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
    VK_CHECK(vkCreateGraphicsPipelines(device_, pipeline_cache_, 1, &pipelineCI, nullptr, &pipeline));
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
    cmdBufAllocateInfo.commandPool = command_pool_;
    cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufAllocateInfo.commandBufferCount = 1;

    VkCommandBuffer cmdBuffer;
    VK_CHECK(vkAllocateCommandBuffers(device_, &cmdBufAllocateInfo, &cmdBuffer));

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
    VK_CHECK(vkCreateFence(device_, &fenceInfo, nullptr, &fence));
    
    // Submit to the queue
    VK_CHECK(vkQueueSubmit(graphics_queue_, 1, &submitInfo, fence));
    // Wait for the fence to signal that command buffer has finished executing
    VK_CHECK(vkWaitForFences(device_, 1, &fence, VK_TRUE, 100000000000));

    vkDestroyFence(device_, fence, nullptr);

    vkFreeCommandBuffers(device_, command_pool_, 1, &cmdBuf);

    vkQueueWaitIdle(graphics_queue_);

    vkDestroyPipeline(device_, pipeline, nullptr);
    vkDestroyPipelineLayout(device_, pipelinelayout, nullptr);
    vkDestroyRenderPass(device_, renderpass, nullptr);
    vkDestroyFramebuffer(device_, framebuffer, nullptr);
    vkDestroyDescriptorSetLayout(device_, descriptorsetlayout, nullptr);

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
    framebuffer_resized_ = true;
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
