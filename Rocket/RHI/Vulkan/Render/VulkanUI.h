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
			Ref<VulkanDevice> device, 
			VkRenderPass renderPass, 
			VkQueue queue, 
			VkPipelineCache pipelineCache, 
			VkSampleCountFlagBits multiSampleCount);
		void Initialize() final;
		void Finalize() final;
		void UpdataOverlay(uint32_t width, uint32_t height);
		void PrepareUI();
		void Draw(VkCommandBuffer cmdBuffer);
		void SetWindowHandle(GLFWwindow* handle) { windowHandle = handle; }
	public:
		Ref<VulkanDevice> device;
		VkRenderPass renderPass;
		VkQueue queue;
		VkPipelineCache pipelineCache;
		VkSampleCountFlagBits multiSampleCount;
		GLFWwindow* windowHandle;

		VulkanBufferStruct vertexBuffer;
		VulkanBufferStruct indexBuffer;
		VulkanTexture2D fontTexture;

		VkPipelineLayout pipelineLayout;
		VkDescriptorPool descriptorPool;
		VkPipeline pipeline;
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorSet descriptorSet;

		struct alignas(16) PushConstBlock
		{
			Vector2f scale;
			Vector2f translate;
		} pushConstBlock;

		bool canDraw = false;

		// UI Control Variable
		ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	};
}