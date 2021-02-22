#pragma once
#include "Core/Core.h"

#include <vulkan/vulkan.h>

typedef struct GLFWwindow GLFWwindow;

namespace Rocket
{
	class VulkanPipeline
	{
	public:
		void Connect(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface);
		void Initialize();

		void SetWindowHandle(GLFWwindow* handle) { windowHandle = handle; }

		void CreateRenderPass();
		void CreateDescriptorSetLayout();
		void CreateGraphicsPipeline();
	public:
		VkRenderPass renderPass;
		VkDescriptorSetLayout descriptorSetLayout;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;
		VkSampleCountFlagBits msaaSamples;

		VkInstance instance;
		VkDevice device;
		VkPhysicalDevice physicalDevice;
		VkSurfaceKHR surface;
		VkSurfaceFormatKHR surfaceFormat;
		VkExtent2D extent = {};

		GLFWwindow* windowHandle;
	};
}