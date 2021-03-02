#pragma once
#include "Core/Core.h"
#include "Vulkan/Render/VulkanFunction.h"

#include <vulkan/vulkan.h>

namespace Rocket
{
	class VulkanDevice
	{
	public:
		operator VkDevice() { return logicalDevice; };

		VulkanDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
		~VulkanDevice() = default;

		void Initialize();
		void Finalize();

		uint32_t GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound = nullptr);
		uint32_t GetQueueFamilyIndex(VkQueueFlagBits queueFlags);
		VkResult CreateLogicalDevice(VkPhysicalDeviceFeatures enabledFeatures, std::vector<const char*> enabledExtensions, bool enableValidationLayers = false, const Vec<const char*>& validationLayers = {}, VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
		VkResult CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, VkBuffer* buffer, VkDeviceMemory* memory, void* data = nullptr);
		VkCommandPool CreateCommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		VkCommandBuffer CreateCommandBuffer(VkCommandBufferLevel level, bool begin = false);
		void BeginCommandBuffer(VkCommandBuffer commandBuffer);
		void FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, bool free = true);

	public:
		VkPhysicalDevice physicalDevice;
		VkSurfaceKHR surface;

		VkDevice logicalDevice;
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceFeatures features;
		VkPhysicalDeviceFeatures enabledFeatures;
		VkPhysicalDeviceMemoryProperties memoryProperties;
		Vec<VkQueueFamilyProperties> queueFamilyProperties;
		VkCommandPool commandPool = VK_NULL_HANDLE;
		QueueFamilyIndices queueFamilyIndices;
		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkQueue computeQueue = VK_NULL_HANDLE;
		VkQueue presentQueue = VK_NULL_HANDLE;
	};
}