#pragma once
#include "Core/Core.h"
#include "Common/Buffer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <vulkan/vulkan.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cstdlib>
#include <optional>
#include <functional>
#include <algorithm>
#include <set>

struct GLFWwindow;

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }

    bool operator==(const Vertex& other) const
    {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

namespace std
{
    template<> struct hash<Vertex>
    {
        size_t operator()(Vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

struct UniformBufferObject 
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    Rocket::Vec<VkSurfaceFormatKHR> formats;
    Rocket::Vec<VkPresentModeKHR> presentModes;
};

namespace Rocket
{
    bool CheckValidationLayerSupport();
    Vec<const char*> GetRequiredExtensions();
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    bool IsDeviceSuitable(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);
    bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device);
    QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const Vec<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const Vec<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window_handle);
    SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);
    VkShaderModule CreateShaderModule(const Buffer& code, const VkDevice& device);
    VkShaderModule CreateShaderModule(const Vec<uint32_t>& code, const VkDevice& device);
    uint32_t FindMemoryType(const VkPhysicalDevice& device, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void CreateImage(const VkDevice& device, const VkPhysicalDevice& physicalDevice, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    VkImageView CreateImageView(const VkDevice& device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
    VkFormat FindSupportedFormat(const VkPhysicalDevice& physicalDevice, const Vec<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat FindDepthFormat(const VkPhysicalDevice& physicalDevice);
    VkSampleCountFlagBits GetMaxUsableSampleCount(const VkPhysicalDevice& physicalDevice);
    bool HasStencilComponent(VkFormat format);
    void CreateBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    VkCommandBuffer BeginSingleTimeCommands(VkDevice& device, VkCommandPool& commandPool);
    void EndSingleTimeCommands(VkDevice& device, VkCommandPool& commandPool, VkQueue& graphicsQueue, VkCommandBuffer commandBuffer);
    void GenerateMipmaps(VkDevice& device, VkCommandPool& commandPool, VkQueue& graphicsQueue, VkPhysicalDevice& physicalDevice, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    void TransitionImageLayout(VkDevice& device, VkCommandPool& commandPool, VkQueue& graphicsQueue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
    void CopyBufferToImage(VkDevice& device, VkCommandPool& commandPool, VkQueue& graphicsQueue,VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void CopyBuffer(VkDevice& device, VkCommandPool& commandPool, VkQueue& graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
}