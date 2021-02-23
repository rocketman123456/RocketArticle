#pragma once
#include "Core/Core.h"
#include "Vulkan/VulkanBuffer.h"
#include "Vulkan/VulkanTexture.h"
#include "Vulkan/VulkanDevice.h"

#include <Vulkan/vulkan.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Rocket
{
	class VulkanUI
	{
	public:
		void Connect(
			Ref<VulkanDevice> device, 
			VkRenderPass renderPass, 
			VkQueue queue, 
			VkPipelineCache pipelineCache, 
			VkSampleCountFlagBits multiSampleCount);
		void Initialize();
		void Finalize();
		void Draw(VkCommandBuffer cmdBuffer);
	public:
		Ref<VulkanDevice> device;
		VkRenderPass renderPass;
		VkQueue queue;
		VkPipelineCache pipelineCache;
		VkSampleCountFlagBits multiSampleCount;

		VulkanBufferStruct vertexBuffer;
		VulkanBufferStruct indexBuffer;
		VulkanTexture2D fontTexture;

		VkPipelineLayout pipelineLayout;
		VkDescriptorPool descriptorPool;
		VkPipeline pipeline;
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorSet descriptorSet;

		struct PushConstBlock
		{
			glm::vec2 scale;
			glm::vec2 translate;
		} pushConstBlock;
	};
}