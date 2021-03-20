#pragma once
#include "Core/Core.h"

#include <vulkan/vulkan.h>

typedef struct GLFWwindow GLFWwindow;

namespace Rocket
{
	typedef struct _SwapChainBuffers_ {
		VkImage image;
		VkImageView view;
	} SwapChainBuffer;

	class VulkanSwapchain
	{
	public:
		void Connect(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface);
		void Initialize(bool vsync = false);
		VkResult AcquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t* imageIndex);
		VkResult QueuePresent(VkQueue queue, uint32_t imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE);
		void Finalize();

		void SetWindowHandle(GLFWwindow* handle) { window_handle = handle; }
	public:
		VkFormat color_format;
		VkColorSpaceKHR color_space;
		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		uint32_t image_count;
		Vec<VkImage> images;
		Vec<SwapChainBuffer> buffers;
		VkExtent2D extent = {};
		uint32_t queue_node_index = UINT32_MAX;

		VkInstance instance;
		VkDevice device;
		VkPhysicalDevice physical_device;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkSurfaceFormatKHR surface_format;

		GLFWwindow* window_handle;
	private:
		// Function pointers
		PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
		PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
		PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
		PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
		PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
		PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
		PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
		PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
		PFN_vkQueuePresentKHR fpQueuePresentKHR;
	};
}