#pragma once
#include "Core/Core.h"
#include "Vulkan/Render/VulkanFunction.h"

#include <vulkan/vulkan.h>

namespace Rocket
{
	struct VulkanDevice
	{
		operator VkDevice() { return logical_device; };

		VulkanDevice(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
		~VulkanDevice() = default;

		void Initialize();
		void Finalize();

		uint32_t GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound = nullptr);
		uint32_t GetQueueFamilyIndex(VkQueueFlagBits queueFlags);
		VkResult CreateLogicalDevice(VkPhysicalDeviceFeatures enabled_features, std::vector<const char*> enabledExtensions, bool enableValidationLayers = false, const Vec<const char*>& validationLayers = {}, VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
		VkResult CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, VkBuffer* buffer, VkDeviceMemory* memory, void* data = nullptr);
		VkCommandPool CreateCommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		VkCommandBuffer CreateCommandBuffer(VkCommandBufferLevel level, bool begin = false);
		void BeginCommandBuffer(VkCommandBuffer commandBuffer);
		void FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, bool free = true);

		VkPhysicalDevice physical_device;
		VkSurfaceKHR surface;

		VkDevice logical_device;
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceFeatures features;
		VkPhysicalDeviceFeatures enabled_features;
		VkPhysicalDeviceMemoryProperties memory_properties;
		Vec<VkQueueFamilyProperties> queue_family_properties;
		VkCommandPool command_pool = VK_NULL_HANDLE;
		QueueFamilyIndices queue_family_indices;
		VkQueue graphics_queue = VK_NULL_HANDLE;
		VkQueue compute_queue = VK_NULL_HANDLE;
		VkQueue present_queue = VK_NULL_HANDLE;
	};
}