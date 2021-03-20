#include "Vulkan/Render/VulkanUI.h"
#include "Vulkan/Render/VulkanFunction.h"
#include "Vulkan/Render/VulkanShader.h"
#include "Module/AssetLoader.h"

// ImGui Implements
#include <backends/imgui_impl_glfw.cpp>
#include <backends/imgui_impl_vulkan.cpp>
#include <GLFW/glfw3.h>

using namespace Rocket;

static void check_vk_result_ui(VkResult err)
{
	if (err == 0)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}

void VulkanUI::Connect(
	VkInstance instance, 
    Ref<VulkanDevice> device, 
    VkRenderPass renderPass, 
    VkQueue queue, 
    VkPipelineCache pipelineCache, 
    VkSampleCountFlagBits multiSampleCount,
	uint32_t frameInFlight)
{
	this->instance = instance;
    this->device = device;
    this->render_pass = renderPass;
    this->queue = queue;
    this->pipeline_cache = pipelineCache;
    this->msaa_samples = multiSampleCount;
	this->frame_in_flight = frameInFlight;
}

void VulkanUI::Initialize()
{
	// Descriptor pool
	{
		std::vector<VkDescriptorPoolSize> poolSizes = {
			//{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 }
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		VkDescriptorPoolCreateInfo descriptorPoolCI{};
		descriptorPoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCI.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		descriptorPoolCI.poolSizeCount = (uint32_t)poolSizes.size();
		descriptorPoolCI.pPoolSizes = poolSizes.data();
		descriptorPoolCI.maxSets = 1000 * (uint32_t)poolSizes.size(); //1;
		VK_CHECK(vkCreateDescriptorPool(device->logical_device, &descriptorPoolCI, nullptr, &descriptor_pool));
	}

	// Setup ImGui
	UI::Initialize();

	// Init Vulkan For ImGui
	{
		ImGui_ImplGlfw_InitForVulkan(window_handle, true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = instance;
		init_info.PhysicalDevice = device->physical_device;
		init_info.Device = device->logical_device;
		init_info.QueueFamily = device->queue_family_indices.graphics_family.value();
		init_info.Queue = device->graphics_queue;
		init_info.PipelineCache = pipeline_cache;
		init_info.DescriptorPool = descriptor_pool;
		init_info.Allocator = nullptr;
		init_info.MinImageCount = frame_in_flight;
		init_info.ImageCount = frame_in_flight;
		init_info.MSAASamples = msaa_samples;
		init_info.CheckVkResultFn = check_vk_result_ui;
		ImGui_ImplVulkan_Init(&init_info, render_pass);
	}

	// Load Fonts
	{
		VkCommandPool command_pool = device->command_pool;
		VkCommandBuffer command_buffer = device->CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, false);
		
		VK_CHECK(vkResetCommandPool(device->logical_device, command_pool, 0));
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VK_CHECK(vkBeginCommandBuffer(command_buffer, &begin_info));
		
		ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
		
		VkSubmitInfo end_info = {};
		end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		end_info.commandBufferCount = 1;
		end_info.pCommandBuffers = &command_buffer;
		VK_CHECK(vkEndCommandBuffer(command_buffer));
		VK_CHECK(vkQueueSubmit(queue, 1, &end_info, VK_NULL_HANDLE));
		
		VK_CHECK(vkDeviceWaitIdle(device->logical_device));
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
}

void VulkanUI::Finalize()
{
	vkDestroyDescriptorPool(device->logical_device, descriptor_pool, nullptr);

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	UI::Finalize();
}

void VulkanUI::UpdataOverlay(uint32_t width, uint32_t height)
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	
	UI::UpdataOverlay(width, height);
}

void VulkanUI::Draw(VkCommandBuffer cmdBuffer)
{
	ImDrawData* imDrawData = ImGui::GetDrawData();
	ImGui_ImplVulkan_RenderDrawData(imDrawData, cmdBuffer);
}
