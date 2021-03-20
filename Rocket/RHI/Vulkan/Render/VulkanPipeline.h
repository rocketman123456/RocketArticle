#pragma once
#include "Core/Core.h"

#include <vulkan/vulkan.h>

typedef struct GLFWwindow GLFWwindow;

namespace Rocket
{
	struct VulkanPipeline
	{
		void Connect(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface);
		void Initialize();
		void Finalize();

		void SetWindowHandle(GLFWwindow* handle) { window_handle = handle; }
		void SetMsaaSample(VkSampleCountFlagBits msaa) { msaa_samples = msaa; }

		void CreateRenderPass();
		void CreateGuiRenderPass();
		void CreateDescriptorSetLayout();
		void CreateGraphicsPipeline();

		VkRenderPass render_pass;
		VkRenderPass gui_render_pass;
		VkDescriptorSetLayout descriptor_set_layout;
		VkPipelineLayout pipeline_layout;
		VkPipeline graphics_pipeline;

		VkInstance instance;
		VkDevice device;
		VkPhysicalDevice physical_device;
		VkSurfaceKHR surface;

		VkSurfaceFormatKHR surface_format;
		VkExtent2D extent = {};

		VkSampleCountFlagBits msaa_samples = VK_SAMPLE_COUNT_1_BIT;
		GLFWwindow* window_handle;
	};
}