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

typedef struct GLFWwindow GLFWwindow;

#define VK_CHECK(f)																						\
{																										\
	VkResult res = (f);																					\
	if (res != VK_SUCCESS)																				\
	{																									\
		RK_GRAPHICS_ERROR("Fatal : VkResult is \" {} \" in {} at line {}", res, __FILE__, __LINE__);	\
		assert(res == VK_SUCCESS);																		\
	}																									\
}

#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                        \
{                                                                       \
	fp##entrypoint = reinterpret_cast<PFN_vk##entrypoint>(vkGetInstanceProcAddr(inst, "vk"#entrypoint)); \
	if (fp##entrypoint == NULL)                                         \
	{																    \
		exit(1);                                                        \
	}                                                                   \
}

#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                           \
{                                                                       \
	fp##entrypoint = reinterpret_cast<PFN_vk##entrypoint>(vkGetDeviceProcAddr(dev, "vk"#entrypoint));   \
	if (fp##entrypoint == NULL)                                         \
	{																    \
		exit(1);                                                        \
	}                                                                   \
}

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
	std::optional<uint32_t> computeFamily;

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

template<class T>
struct Resource
{
	T resource;
	VkDeviceMemory memory;
	VkDeviceSize allocationSize;
	uint32_t memoryTypeIndex;
};

struct VulkanBuffer
{
	VkBuffer buffer;
	VkDeviceMemory memory;
	void* data;
	size_t size;
};

struct VulkanImage
{
	VkImage image;
	VkImageView imageView;
	VkDeviceMemory memory;
};

struct VulkanSwapchain
{
	VkSwapchainKHR swapchain;
	Rocket::Vec<VkImage> images;
	uint32_t width, height;
	uint32_t imageCount;
};

enum SwapchainStatus
{
	Swapchain_Ready,
	Swapchain_Resized,
	Swapchain_NotReady,
};

// TODO : update vulkan shader class
//struct Vulkan_Shader
//{
//	VkShaderModule module;
//	VkShaderStageFlagBits stage;
//
//	VkDescriptorType resourceTypes[32];
//	uint32_t resourceMask;
//
//	uint32_t localSizeX;
//	uint32_t localSizeY;
//	uint32_t localSizeZ;
//
//	bool usesPushConstants;
//};

struct VulkanProgram
{
	VkPipelineBindPoint bindPoint;
	VkPipelineLayout layout;
	VkDescriptorSetLayout setLayout;
	VkDescriptorUpdateTemplate updateTemplate;
	VkShaderStageFlags pushConstantStages;
};

struct DescriptorInfo
{
	union
	{
		VkDescriptorImageInfo image;
		VkDescriptorBufferInfo buffer;
	};

	DescriptorInfo() {}

	DescriptorInfo(VkImageView imageView, VkImageLayout imageLayout)
	{
		image.sampler = VK_NULL_HANDLE;
		image.imageView = imageView;
		image.imageLayout = imageLayout;
	}

	DescriptorInfo(VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout)
	{
		image.sampler = sampler;
		image.imageView = imageView;
		image.imageLayout = imageLayout;
	}

	DescriptorInfo(VkBuffer buffer_, VkDeviceSize offset, VkDeviceSize range)
	{
		buffer.buffer = buffer_;
		buffer.offset = offset;
		buffer.range = range;
	}

	DescriptorInfo(VkBuffer buffer_)
	{
		buffer.buffer = buffer_;
		buffer.offset = 0;
		buffer.range = VK_WHOLE_SIZE;
	}
};

//using Shaders = std::initializer_list<const Vulkan_Shader*>;
using Constants = std::initializer_list<int>;

// niagara ----------------------//
VkSemaphore CreateSemaphore(VkDevice device);
VkCommandPool CreateCommandPool(
	VkDevice device, 
	uint32_t familyIndex);

VkRenderPass CreateRenderPass(
	VkDevice device, 
	VkFormat colorFormat, 
	VkFormat depthFormat, 
	bool late);

VkFramebuffer CreateFramebuffer(
	VkDevice device, 
	VkRenderPass renderPass, 
	VkImageView colorView, 
	VkImageView depthView, 
	uint32_t width, 
	uint32_t height);

VkQueryPool CreateQueryPool(
	VkDevice device, 
	uint32_t queryCount, 
	VkQueryType queryType);

// Resource
VkImageMemoryBarrier ImageBarrier(
	VkImage image, 
	VkAccessFlags srcAccessMask, 
	VkAccessFlags dstAccessMask, 
	VkImageLayout oldLayout, 
	VkImageLayout newLayout, 
	VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT);

VkBufferMemoryBarrier BufferBarrier(
	VkBuffer buffer, 
	VkAccessFlags srcAccessMask, 
	VkAccessFlags dstAccessMask);

void CreateBuffer(
	VulkanBuffer& result, 
	VkDevice device, 
	const VkPhysicalDeviceMemoryProperties& memoryProperties, 
	size_t size, 
	VkBufferUsageFlags usage, 
	VkMemoryPropertyFlags memoryFlags);

void UploadBuffer(
	VkDevice device, 
	VkCommandPool commandPool, 
	VkCommandBuffer commandBuffer, 
	VkQueue queue, 
	const VulkanBuffer& buffer, 
	const VulkanBuffer& scratch, 
	const void* data, size_t size);

void DestroyBuffer(
	const VulkanBuffer& buffer, 
	VkDevice device);

VkImageView CreateImageView(
	VkDevice device, 
	VkImage image, 
	VkFormat format, 
	uint32_t mipLevel, 
	uint32_t levelCount);

void CreateImage(
	VulkanImage& result, 
	VkDevice device, 
	const VkPhysicalDeviceMemoryProperties& memoryProperties, 
	uint32_t width, 
	uint32_t height, 
	uint32_t mipLevels, 
	VkFormat format, 
	VkImageUsageFlags usage);

void DestroyImage(
	const VulkanImage& image, 
	VkDevice device);

uint32_t GetImageMipLevels(
	uint32_t width, 
	uint32_t height);

VkSampler CreateSampler(
	VkDevice device, 
	VkSamplerReductionModeEXT reductionMode);

// Swapchain
// VkSurfaceKHR CreateSurface(VkInstance instance, GLFWwindow* window);
VkFormat GetSwapchainFormat(
	VkPhysicalDevice physicalDevice, 
	VkSurfaceKHR surface);

void CreateSwapchain(
	VulkanSwapchain& result, 
	VkPhysicalDevice physicalDevice, 
	VkDevice device, 
	VkSurfaceKHR surface, 
	uint32_t familyIndex, 
	VkFormat format, 
	VkSwapchainKHR oldSwapchain = 0);

void DestroySwapchain(
	VkDevice device, 
	const VulkanSwapchain& swapchain);

SwapchainStatus UpdateSwapchain(
	VulkanSwapchain& result, 
	VkPhysicalDevice physicalDevice, 
	VkDevice device, 
	VkSurfaceKHR surface, 
	uint32_t familyIndex, 
	VkFormat format);

// Device
VkDevice CreateDevice(
	VkInstance instance, 
	VkPhysicalDevice physicalDevice, 
	uint32_t familyIndex, 
	bool pushDescriptorsSupported, 
	bool checkpointsSupported, 
	bool meshShadingSupported);

// Shader
//VkPipeline CreateGraphicsPipeline(
//	VkDevice device, 
//	VkPipelineCache pipelineCache, 
//	VkRenderPass renderPass, 
//	Shaders shaders, 
//	VkPipelineLayout layout, 
//	Constants constants = {});
//
//VkPipeline CreateComputePipeline(
//	VkDevice device, 
//	VkPipelineCache pipelineCache, 
//	const Vulkan_Shader& shader, 
//	VkPipelineLayout layout, 
//	Constants constants = {});
//
//VulkanProgram CreateProgram(
//	VkDevice device, 
//	VkPipelineBindPoint bindPoint, 
//	Shaders shaders, 
//	size_t pushConstantSize, 
//	bool pushDescriptorsSupported);
//
//void DestroyProgram(
//	VkDevice device, 
//	const VulkanProgram& program);
// niagara ----------------------//

bool CheckValidationLayerSupport();

Rocket::Vec<const char*> GetRequiredExtensions();

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

bool IsDeviceSuitable(
	const VkPhysicalDevice& device, 
	const VkSurfaceKHR& surface);

bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device);

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const Rocket::Vec<VkSurfaceFormatKHR>& availableFormats);

VkPresentModeKHR ChooseSwapPresentMode(const Rocket::Vec<VkPresentModeKHR>& availablePresentModes);

VkExtent2D ChooseSwapExtent(
	const VkSurfaceCapabilitiesKHR& capabilities, 
	GLFWwindow* window_handle);

SwapChainSupportDetails QuerySwapChainSupport(
	const VkPhysicalDevice& device, 
	const VkSurfaceKHR& surface);

VkShaderModule CreateShaderModule(
	const Rocket::Buffer& code, 
	const VkDevice& device);

VkShaderModule CreateShaderModule(
	const Rocket::Vec<uint32_t>& code, 
	const VkDevice& device);

void CreateImage(
	const VkDevice& device, 
	const VkPhysicalDevice& physicalDevice, 
	uint32_t width, 
	uint32_t height, 
	uint32_t mipLevels, 
	VkSampleCountFlagBits numSamples, 
	VkFormat format, 
	VkImageTiling tiling, 
	VkImageUsageFlags usage, 
	VkMemoryPropertyFlags properties, 
	VkImage& image, 
	VkDeviceMemory& imageMemory);

VkImageView CreateImageView(
	const VkDevice& device, 
	VkImage image, 
	VkFormat format, 
	VkImageAspectFlags aspectFlags, 
	uint32_t mipLevel, 
	uint32_t levelCount);

void CreateBuffer(
	const VkDevice& device, 
	const VkPhysicalDevice& physicalDevice, 
	VkDeviceSize size, 
	VkBufferUsageFlags usage, 
	VkMemoryPropertyFlags properties, 
	VkBuffer& buffer, 
	VkDeviceMemory& bufferMemory);

QueueFamilyIndices FindQueueFamilies(
	const VkPhysicalDevice& device, 
	const VkSurfaceKHR& surface);

uint32_t FindMemoryType(
	const VkPhysicalDevice& device, 
	uint32_t typeFilter, 
	VkMemoryPropertyFlags properties);

VkFormat FindSupportedFormat(
	const VkPhysicalDevice& physicalDevice, 
	const Rocket::Vec<VkFormat>& candidates, 
	VkImageTiling tiling, 
	VkFormatFeatureFlags features);

VkFormat FindDepthFormat(const VkPhysicalDevice& physicalDevice);

VkSampleCountFlagBits GetMaxUsableSampleCount(const VkPhysicalDevice& physicalDevice);

bool HasStencilComponent(VkFormat format);

VkCommandBuffer BeginSingleTimeCommands(
	VkDevice& device, 
	VkCommandPool& commandPool);

void EndSingleTimeCommands(
	VkDevice& device, 
	VkCommandPool& commandPool, 
	VkQueue& graphicsQueue, 
	VkCommandBuffer commandBuffer);

void GenerateMipmaps(
	VkDevice& device, 
	VkCommandPool& commandPool, 
	VkQueue& graphicsQueue, 
	VkPhysicalDevice& physicalDevice, 
	VkImage image, 
	VkFormat imageFormat, 
	int32_t texWidth, 
	int32_t texHeight, 
	uint32_t mipLevels);

void TransitionImageLayout(
	VkDevice& device, 
	VkCommandPool& commandPool, 
	VkQueue& graphicsQueue, 
	VkImage image, 
	VkFormat format, 
	VkImageLayout oldLayout, 
	VkImageLayout newLayout, 
	uint32_t mipLevels);

void CopyBufferToImage(
	VkDevice& device, 
	VkCommandPool& commandPool, 
	VkQueue& graphicsQueue,
	VkBuffer buffer, 
	VkImage image, 
	uint32_t width,
	uint32_t height);

void CopyBuffer(
	VkDevice& device, 
	VkCommandPool& commandPool, 
	VkQueue& graphicsQueue, 
	VkBuffer srcBuffer, 
	VkBuffer dstBuffer, 
	VkDeviceSize size);

// Vulkan PBR
uint32_t GetMemoryType(
	uint32_t typeBits, 
	VkMemoryPropertyFlags properties,
	VkPhysicalDeviceMemoryProperties memoryProperties,
	VkBool32 *memTypeFound = nullptr);