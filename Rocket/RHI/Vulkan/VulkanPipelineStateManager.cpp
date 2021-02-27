#include "Vulkan/VulkanPipelineStateManager.h"
#include "Vulkan/VulkanGraphicsManager.h"
#include "Vulkan/VulkanShader.h"
#include "Module/WindowManager.h"

#include <memory>
#include <shaderc/shaderc.hpp>

using namespace Rocket;

PipelineStateManager* Rocket::GetPipelineStateManager() { return new VulkanPipelineStateManager(); }

static VkFormat ShaderDataTypeToVulkanBaseType(ShaderDataType type)
{
    switch (type)
    {
    case ShaderDataType::Float: return VK_FORMAT_R32_SFLOAT;
    case ShaderDataType::Vec2f: return VK_FORMAT_R32G32_SFLOAT;
    case ShaderDataType::Vec3f: return VK_FORMAT_R32G32B32_SFLOAT;
    case ShaderDataType::Vec4f: return VK_FORMAT_R32G32B32A32_SFLOAT;
    case ShaderDataType::Mat2f: return VK_FORMAT_R32_SFLOAT;
    case ShaderDataType::Mat3f: return VK_FORMAT_R32_SFLOAT;
    case ShaderDataType::Mat4f: return VK_FORMAT_R32_SFLOAT;
    case ShaderDataType::Int: return VK_FORMAT_R32_UINT;
    case ShaderDataType::Vec2i: return VK_FORMAT_R32G32_UINT;
    case ShaderDataType::Vec3i: return VK_FORMAT_R32G32B32_UINT;
    case ShaderDataType::Vec4i: return VK_FORMAT_R32G32B32A32_UINT;
    case ShaderDataType::Bool: return VK_FORMAT_R8_UINT;
    default: RK_CORE_ASSERT(false, "Unknown ShaderDataType!");
    }
}

bool VulkanPipelineStateManager::InitializePipelineState(PipelineState** ppPipelineState)
{
    VulkanPipelineState* pnew_state =new VulkanPipelineState(**ppPipelineState);

    ShaderSourceList list;

    if (!(*ppPipelineState)->vertexShaderName.empty()) {
        list.emplace_back(shaderc_glsl_vertex_shader, (*ppPipelineState)->vertexShaderName);
    }

    if (!(*ppPipelineState)->pixelShaderName.empty()) {
        list.emplace_back(shaderc_glsl_fragment_shader, (*ppPipelineState)->pixelShaderName);
    }

    if (!(*ppPipelineState)->geometryShaderName.empty()) {
        list.emplace_back(shaderc_glsl_geometry_shader, (*ppPipelineState)->geometryShaderName);
    }

    if (!(*ppPipelineState)->computeShaderName.empty()) {
        list.emplace_back(shaderc_glsl_compute_shader, (*ppPipelineState)->computeShaderName);
    }

    if (!(*ppPipelineState)->tessControlShaderName.empty()) {
        list.emplace_back(shaderc_glsl_tess_control_shader, (*ppPipelineState)->tessControlShaderName);
    }

    if (!(*ppPipelineState)->tessEvaluateShaderName.empty()) {
        list.emplace_back(shaderc_glsl_tess_evaluation_shader, (*ppPipelineState)->tessEvaluateShaderName);
    }

    VulkanGraphicsManager* vulkan_graphics_manager = dynamic_cast<VulkanGraphicsManager*>(g_GraphicsManager);
    pnew_state->m_DeviceHandle = vulkan_graphics_manager->GetDevice();
    VkPhysicalDevice physicalDevice = vulkan_graphics_manager->GetPhysicalDevice();
    VkSurfaceKHR surface = vulkan_graphics_manager->GetSurface();
    VkSampleCountFlagBits msaaSamples = GetMaxUsableSampleCount(physicalDevice);
    GLFWwindow* windowHandle = static_cast<GLFWwindow*>(g_WindowManager->GetNativeWindow());

    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice, surface);
    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, windowHandle);
    VkFormat swapChainImageFormat = surfaceFormat.format;

    VkRenderPassCreateInfo renderPassInfo{};
    VkSubpassDescription subpass{};
    VkSubpassDependency dependency{};

    // Create Render Pass Info
    if (pnew_state->renderTarget == RENDER_TARGET::NONE)
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = msaaSamples;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = FindDepthFormat(physicalDevice);
        depthAttachment.samples = msaaSamples;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachmentResolve{};
        colorAttachmentResolve.format = swapChainImageFormat;
        colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentResolveRef{};
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        subpass.pResolveAttachments = &colorAttachmentResolveRef;

        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
    }
    else
    {
        Vec<VkAttachmentDescription> attachments;
        Vec<VkAttachmentReference> colorAttachmentRefs;
        VkAttachmentReference depthAttachmentRef;
        VkAttachmentReference colorAttachmentResolveRef{};
        VkAccessFlags attachmentFlag = 0x00000000;

        auto frameBufferInfo = pnew_state->frameBufferInfo;

        uint32_t index = 0;

        if (frameBufferInfo.ColorAttachment.size() != 0)
        {
            for (auto color : frameBufferInfo.ColorAttachment)
            {
                VkAttachmentDescription colorAttachment{};
                colorAttachment.format = swapChainImageFormat;
                colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachments.push_back(colorAttachment);

                VkAttachmentReference colorAttachmentRef{};
                colorAttachmentRef.attachment = index;
                colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                colorAttachmentRefs.push_back(colorAttachmentRef);

                index++;
            }

            attachmentFlag = attachmentFlag | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        }

        if (frameBufferInfo.DepthAttachment != FRAME_TEXTURE_FORMAT::NONE)
        {
            VkAttachmentDescription depthAttachment{};
            depthAttachment.format = FindDepthFormat(physicalDevice);
            depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            attachments.push_back(depthAttachment);

            depthAttachmentRef.attachment = index;
            depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            index++;

            attachmentFlag = attachmentFlag | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        }

        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = frameBufferInfo.ColorAttachment.size();
        subpass.pColorAttachments = colorAttachmentRefs.data();
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        subpass.pResolveAttachments = nullptr;

        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = attachmentFlag;

        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
    }

    //if (vkCreateRenderPass(pnew_state->m_DeviceHandle, &renderPassInfo, nullptr, &pnew_state->m_RenderPass) != VK_SUCCESS)
    //{
    //    RK_GRAPHICS_ERROR("failed to create render pass!");
    //    return false;
    //}

    auto ubolayouts = pnew_state->uniformLayout;

    Vec<VkDescriptorSetLayoutBinding> bindings;
    for (auto ubo : ubolayouts)
    {
        VkDescriptorSetLayoutBinding bind{};
        bind.binding = ubo.Binding;
        bind.descriptorCount = ubo.Count;
        bind.descriptorType = (VkDescriptorType)ubo.UniformType;
        bind.pImmutableSamplers = nullptr;
        bind.stageFlags = (VkShaderStageFlags)ubo.ShaderStage;
        bindings.push_back(bind);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    //if (vkCreateDescriptorSetLayout(pnew_state->m_DeviceHandle, &layoutInfo, nullptr, &pnew_state->m_DescriptorSetLayout) != VK_SUCCESS)
    //{
    //    RK_GRAPHICS_ERROR("failed to create descriptor set layout!");
    //    vkDestroyRenderPass(pnew_state->m_DeviceHandle, pnew_state->m_RenderPass, nullptr);
    //    return false;
    //}

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &pnew_state->m_DescriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    //if (vkCreatePipelineLayout(pnew_state->m_DeviceHandle, &pipelineLayoutInfo, nullptr, &pnew_state->m_PipelineLayout) != VK_SUCCESS)
    //{
    //    RK_GRAPHICS_ERROR("failed to create pipeline layout!");
    //    vkDestroyRenderPass(pnew_state->m_DeviceHandle, pnew_state->m_RenderPass, nullptr);
    //    vkDestroyDescriptorSetLayout(pnew_state->m_DeviceHandle, pnew_state->m_DescriptorSetLayout, nullptr);
    //    return false;
    //}
    
    String name = pnew_state->pipelineStateName + " Shader";
    auto shaderProgram = CreateRef<VulkanShader>(name);
    shaderProgram->SetDevice(pnew_state->m_DeviceHandle);
    bool result = shaderProgram->Initialize(list);
    pnew_state->shaderProgram = shaderProgram;

    auto layouts = pnew_state->bufferLayout;

    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = layouts.GetStride();
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    
    Vec<VkVertexInputAttributeDescription> attributeDescriptions;
    for (auto layout : layouts)
    {
        attributeDescriptions.push_back({ layout.Index, 0, ShaderDataTypeToVulkanBaseType(layout.Type), (uint32_t)layout.Offset });
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = extent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

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
    multisampling.rasterizationSamples = msaaSamples;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;
    pipelineInfo.stageCount = static_cast<uint32_t>(shaderProgram->GetShaderInfo().size());
    pipelineInfo.pStages = shaderProgram->GetShaderInfo().data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pTessellationState = nullptr;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr;
    pipelineInfo.layout = pnew_state->m_PipelineLayout;
    pipelineInfo.renderPass = pnew_state->m_RenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    //if (vkCreateGraphicsPipelines(pnew_state->m_DeviceHandle, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pnew_state->m_GraphicsPipeline) != VK_SUCCESS)
    //{
    //    RK_GRAPHICS_ERROR("failed to create graphics pipeline!");
    //    shaderProgram->Finalize();
    //    vkDestroyPipelineLayout(pnew_state->m_DeviceHandle, pnew_state->m_PipelineLayout, nullptr);
    //    vkDestroyRenderPass(pnew_state->m_DeviceHandle, pnew_state->m_RenderPass, nullptr);
    //    vkDestroyDescriptorSetLayout(pnew_state->m_DeviceHandle, pnew_state->m_DescriptorSetLayout, nullptr);
    //    return false;
    //}

    shaderProgram->Finalize();

    *ppPipelineState = pnew_state;
    return true;
}


void VulkanPipelineStateManager::DestroyPipelineState(PipelineState& pipelineState)
{
    VulkanPipelineState* pPipelineState = dynamic_cast<VulkanPipelineState*>(&pipelineState);

    //vkDestroyPipeline(pPipelineState->m_DeviceHandle, pPipelineState->m_GraphicsPipeline, nullptr);
    //vkDestroyPipelineLayout(pPipelineState->m_DeviceHandle, pPipelineState->m_PipelineLayout, nullptr);
    //vkDestroyRenderPass(pPipelineState->m_DeviceHandle, pPipelineState->m_RenderPass, nullptr);
    //vkDestroyDescriptorSetLayout(pPipelineState->m_DeviceHandle, pPipelineState->m_DescriptorSetLayout, nullptr);
}