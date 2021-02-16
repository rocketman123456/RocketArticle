#include "Vulkan/VulkanTexture.h"

#include <stb_image.h>
#include <vulkan/vulkan.h>

using namespace Rocket;

VulkanTexture2D::VulkanTexture2D(uint32_t width, uint32_t height)
{

}

VulkanTexture2D::VulkanTexture2D(String path)
{
    //int texWidth, texHeight, texChannels;
    //stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    //VkDeviceSize imageSize = texWidth * texHeight * 4;
    //m_MipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
//
    //if (!pixels)
    //    RK_GRAPHICS_ERROR("failed to load texture image!");
//
    //VkBuffer stagingBuffer;
    //VkDeviceMemory stagingBufferMemory;
    //CreateBuffer(m_Device, m_PhysicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//
    //void* data;
    //vkMapMemory(m_Device, stagingBufferMemory, 0, imageSize, 0, &data);
    //memcpy(data, pixels, static_cast<size_t>(imageSize));
    //vkUnmapMemory(m_Device, stagingBufferMemory);
//
    //stbi_image_free(pixels);
//
    //CreateImage(m_Device, m_PhysicalDevice, texWidth, texHeight, m_MipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);
//
    //transitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
    //copyBufferToImage(stagingBuffer, m_TextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    ////transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps
//
    //vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
    //vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
//
    //generateMipmaps(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);
}

VulkanTexture2D::~VulkanTexture2D()
{

}

void VulkanTexture2D::SetData(void* data, uint32_t size)
{

}

void VulkanTexture2D::Bind(uint32_t slot) const
{

}

void VulkanTexture2D::Unbind(uint32_t slot) const
{

}