#pragma once
#include "Core/Core.h"
#include "Render/DrawBasic/FrameBuffer.h"

#include <vulkan/vulkan.h>

typedef struct GLFWwindow GLFWwindow;

namespace Rocket
{
	class VulkanFrameBuffer
	{
	public:
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

		void SetWindowHandle(GLFWwindow* handle) { windowHandle = handle; }
		void SetMsaaSample(VkSampleCountFlagBits msaa) { msaaSamples = msaa; }

	public:
		Vec<VkFramebuffer> swapChainFramebuffers;

		VkImage colorImage;
		VkDeviceMemory colorImageMemory;
		VkImageView colorImageView;

		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;

		VkDevice device;
		VkRenderPass renderPass;
		Vec<VkImageView> swapChainImageViews;
		VkPhysicalDevice physicalDevice;
		VkSurfaceKHR surface;

		VkSurfaceFormatKHR surfaceFormat;
		VkExtent2D extent = {};

		VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
		GLFWwindow* windowHandle;
	};
}