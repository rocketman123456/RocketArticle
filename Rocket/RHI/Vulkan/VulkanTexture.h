#pragma once
#include "Render/DrawBasic/Texture.h"
#include "Vulkan/VulkanDevice.h"

#include <vulkan/vulkan.h>
#include <stb_image.h>
// Only support DDS, KTX or KMG
#include <gli/gli.hpp>

namespace Rocket
{
    class VulkanTexture
    {
    public:
        Ref<VulkanDevice> device;
        VkImage image = VK_NULL_HANDLE;
        VkImageLayout imageLayout;
        VkDeviceMemory deviceMemory;
        VkImageView view;
        uint32_t width, height;
        uint32_t mipLevels;
        uint32_t layerCount;
        VkDescriptorImageInfo descriptor;
        VkSampler sampler;

        void UpdateDescriptor();
        void Finalize();
    };

    class VulkanTexture2D : implements VulkanTexture
    {
    public:
        void LoadFromFile(
            String filename,
            VkFormat format,
            Ref<VulkanDevice> device,
            VkQueue copyQueue,
            VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        void LoadFromBuffer(
            void* buffer,
            VkDeviceSize bufferSize,
            VkFormat format,
            uint32_t width,
            uint32_t height,
            Ref<VulkanDevice> device,
            VkQueue copyQueue,
            VkFilter filter = VK_FILTER_LINEAR,
            VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    };

    class TextureCubeMap : public VulkanTexture
    {
    public:
        void LoadFromFile(
            String filename,
            VkFormat format,
            Ref<VulkanDevice> device,
            VkQueue copyQueue,
            VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    };
}