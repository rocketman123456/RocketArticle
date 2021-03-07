#pragma once
#include "Core/Core.h"
#include "Render/DrawBasic/UI.h"
#include "Vulkan/Render/VulkanBuffer.h"
#include "Vulkan/Render/VulkanTexture.h"
#include "Vulkan/Render/VulkanDevice.h"

#include <vulkan/vulkan.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

typedef struct GLFWwindow GLFWwindow;

namespace Rocket
{
	class VulkanUI : implements UI
	{
	public:
		void Connect(
			VkInstance instance,
			Ref<VulkanDevice> device, 
			VkRenderPass renderPass, 
			VkQueue queue, 
			VkPipelineCache pipelineCache, 
			VkSampleCountFlagBits multiSampleCount,
			uint32_t frameInFlight);
		void SetWindowHandle(GLFWwindow* handle) { windowHandle = handle; }
		
		void Initialize() final;
		void Finalize() final;
		void UpdataOverlay(uint32_t width, uint32_t height) final;
		void Draw() final {}; // TODO : finish ui draw
		void Draw(VkCommandBuffer cmdBuffer);
		//void PostAction();
	public:
		VkInstance instance;
		Ref<VulkanDevice> device;
		VkRenderPass renderPass;
		VkQueue queue;
		VkPipelineCache pipelineCache;
		VkSampleCountFlagBits multiSampleCount;
		GLFWwindow* windowHandle;
		uint32_t frameInFlight;

		VkDescriptorPool descriptorPool;
	};
}