#include "Vulkan/VulkanFrameBuffer.h"
#include "Vulkan/VulkanFunction.h"

using namespace Rocket;

void VulkanFrameBuffer::Connect(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    VkRenderPass renderPass,
    const Vec<VkImageView>& swapChainImageViews)
{
    this->device = device;
    this->physicalDevice = physicalDevice;
    this->surface = surface;
    this->renderPass = renderPass;
    this->swapChainImageViews.resize(swapChainImageViews.size());
    for (uint32_t i = 0; i < swapChainImageViews.size(); ++i)
    {
        this->swapChainImageViews[i] = swapChainImageViews[i];
    }

    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice, surface);
    surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    extent = ChooseSwapExtent(swapChainSupport.capabilities, windowHandle);
}

void VulkanFrameBuffer::Initialize()
{
    CreateColorResources();
    CreateDepthResources();
    CreateFramebuffers();
}

void VulkanFrameBuffer::Finalize()
{
    vkDestroyImageView(device, depthImageView, nullptr);
    vkDestroyImage(device, depthImage, nullptr);
    vkFreeMemory(device, depthImageMemory, nullptr);

    vkDestroyImageView(device, colorImageView, nullptr);
    vkDestroyImage(device, colorImage, nullptr);
    vkFreeMemory(device, colorImageMemory, nullptr);

    for (auto framebuffer : swapChainFramebuffers)
        vkDestroyFramebuffer(device, framebuffer, nullptr);
}

void VulkanFrameBuffer::CreateColorResources()
{
    VkFormat colorFormat = surfaceFormat.format;

    CreateImage(
        device,
        physicalDevice,
        extent.width,
        extent.height,
        1,
        msaaSamples,
        colorFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        colorImage,
        colorImageMemory
    );
    colorImageView = CreateImageView(device, colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1);
}

void VulkanFrameBuffer::CreateDepthResources()
{
    VkFormat depthFormat = FindDepthFormat(physicalDevice);

    CreateImage(
        device,
        physicalDevice,
        extent.width,
        extent.height,
        1,
        msaaSamples,
        depthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        depthImage,
        depthImageMemory
    );
    depthImageView = CreateImageView(device, depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1);
}

void VulkanFrameBuffer::CreateFramebuffers()
{
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++)
    {
        Vec<VkImageView> attachments = {};

        if (msaaSamples == VK_SAMPLE_COUNT_1_BIT)
        {
            attachments.push_back(swapChainImageViews[i]);
            attachments.push_back(depthImageView);
        }
        else
        {
            attachments.push_back(colorImageView);
            attachments.push_back(depthImageView);
            attachments.push_back(swapChainImageViews[i]);
        }

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
            RK_GRAPHICS_ERROR("failed to create framebuffer!");
    }
}
