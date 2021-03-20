#include "Vulkan/Render/VulkanFrameBuffer.h"
#include "Vulkan/Render/VulkanFunction.h"

using namespace Rocket;

void VulkanFrameBuffer::Connect(
    VkDevice device,
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    VkRenderPass renderPass,
    const Vec<VkImageView>& swapChainImageViews)
{
    this->device = device;
    this->physical_device = physical_device;
    this->surface = surface;
    this->render_pass = renderPass;
    this->swapchain_image_views.resize(swapChainImageViews.size());
    for (uint32_t i = 0; i < swapChainImageViews.size(); ++i)
    {
        this->swapchain_image_views[i] = swapChainImageViews[i];
    }

    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physical_device, surface);
    surface_format = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    extent = ChooseSwapExtent(swapChainSupport.capabilities, window_handle);
}

void VulkanFrameBuffer::Initialize()
{
    CreateColorResources();
    CreateDepthResources();
    CreateFramebuffers();
}

void VulkanFrameBuffer::Finalize()
{
    vkDestroyImageView(device, depth_image_view, nullptr);
    vkDestroyImage(device, depth_image, nullptr);
    vkFreeMemory(device, depth_image_memory, nullptr);

    vkDestroyImageView(device, color_image_view, nullptr);
    vkDestroyImage(device, color_image, nullptr);
    vkFreeMemory(device, color_image_memory, nullptr);

    for (auto framebuffer : swapchain_framebuffers)
        vkDestroyFramebuffer(device, framebuffer, nullptr);
}

void VulkanFrameBuffer::CreateColorResources()
{
    VkFormat colorFormat = surface_format.format;

    CreateImage(
        device,
        physical_device,
        extent.width,
        extent.height,
        1,
        msaa_samples,
        colorFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        color_image,
        color_image_memory
    );
    color_image_view = CreateImageView(device, color_image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1);
}

void VulkanFrameBuffer::CreateDepthResources()
{
    VkFormat depthFormat = FindDepthFormat(physical_device);

    CreateImage(
        device,
        physical_device,
        extent.width,
        extent.height,
        1,
        msaa_samples,
        depthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        depth_image,
        depth_image_memory
    );
    depth_image_view = CreateImageView(device, depth_image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1);
}

void VulkanFrameBuffer::CreateFramebuffers()
{
    swapchain_framebuffers.resize(swapchain_image_views.size());

    for (size_t i = 0; i < swapchain_image_views.size(); i++)
    {
        Vec<VkImageView> attachments = {};

        if (msaa_samples == VK_SAMPLE_COUNT_1_BIT)
        {
            attachments.push_back(swapchain_image_views[i]);
            attachments.push_back(depth_image_view);
        }
        else
        {
            attachments.push_back(color_image_view);
            attachments.push_back(depth_image_view);
            attachments.push_back(swapchain_image_views[i]);
        }

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = render_pass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapchain_framebuffers[i]) != VK_SUCCESS)
            RK_GRAPHICS_ERROR("failed to create framebuffer!");
    }
}
