#pragma once
#include "Core/Core.h"
#include "Render/DrawBasic/FrameBuffer.h"

#include <vulkan/vulkan.h>

typedef struct GLFWwindow GLFWwindow;

namespace Rocket
{
	struct VulkanFrameBuffer
	{
		void Connect(
			VkDevice device, 
			VkPhysicalDevice physicalDevice,
			VkSurfaceKHR surface,
			VkRenderPass renderPass,  
			const Vec<VkImageView>& swapChainImageViews);
		void Initialize();
		void Finalize();

		void CreateColorResources();
		void CreateDepthResources();
		void CreateFramebuffers();

		void SetWindowHandle(GLFWwindow* handle) { window_handle = handle; }
		void SetMsaaSample(VkSampleCountFlagBits msaa) { msaa_samples = msaa; }

		Vec<VkFramebuffer> swapchain_framebuffers;

		VkImage color_image;
		VkDeviceMemory color_image_memory;
		VkImageView color_image_view;

		VkImage depth_image;
		VkDeviceMemory depth_image_memory;
		VkImageView depth_image_view;

		VkDevice device;
		VkRenderPass render_pass;
		Vec<VkImageView> swapchain_image_views;
		VkPhysicalDevice physical_device;
		VkSurfaceKHR surface;

		VkSurfaceFormatKHR surface_format;
		VkExtent2D extent = {};

		VkSampleCountFlagBits msaa_samples = VK_SAMPLE_COUNT_1_BIT;
		GLFWwindow* window_handle;
	};
}