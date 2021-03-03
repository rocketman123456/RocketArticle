#include "Vulkan/Render/VulkanUI.h"
#include "Vulkan/Render/VulkanFunction.h"
#include "Vulkan/Render/VulkanShader.h"
#include "Module/AssetLoader.h"

// ImGui Implements
#include <imgui_internal.h>
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
    this->renderPass = renderPass;
    this->queue = queue;
    this->pipelineCache = pipelineCache;
    this->multiSampleCount = multiSampleCount;
	this->frameInFlight = frameInFlight;
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
		VK_CHECK(vkCreateDescriptorPool(device->logicalDevice, &descriptorPoolCI, nullptr, &descriptorPool));
	}

	// Setup ImGui
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking
		// TODO : fix multi-viewort error
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows 
		// can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
	}

	// Init Vulkan For ImGui
	{
		ImGui_ImplGlfw_InitForVulkan(windowHandle, false);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = instance;
		init_info.PhysicalDevice = device->physicalDevice;
		init_info.Device = device->logicalDevice;
		init_info.QueueFamily = device->queueFamilyIndices.graphicsFamily.value();
		init_info.Queue = device->graphicsQueue;
		init_info.PipelineCache = pipelineCache;
		init_info.DescriptorPool = descriptorPool;
		init_info.Allocator = nullptr;
		init_info.MinImageCount = frameInFlight;
		init_info.ImageCount = frameInFlight;
		init_info.MSAASamples = multiSampleCount;
		init_info.CheckVkResultFn = check_vk_result_ui;
		ImGui_ImplVulkan_Init(&init_info, renderPass);
	}

	// Load Fonts
	{
		VkCommandPool command_pool = device->commandPool;
		VkCommandBuffer command_buffer = device->CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, false);
		
		VK_CHECK(vkResetCommandPool(device->logicalDevice, command_pool, 0));
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
		
		VK_CHECK(vkDeviceWaitIdle(device->logicalDevice));
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
}

void VulkanUI::Finalize()
{
	vkDestroyDescriptorPool(device->logicalDevice, descriptorPool, nullptr);

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void VulkanUI::UpdataOverlay(uint32_t width, uint32_t height)
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	
	ImGuiContext* context = ImGui::GetCurrentContext();
	//RK_CORE_INFO("ImGui Info: {}, {}", context->FrameCount, context->FrameCountPlatformEnded);

	ImGui::NewFrame();
	//RK_CORE_INFO("NewFrame Info: {}, {}", context->FrameCount, context->FrameCountPlatformEnded);

	ImGui::Begin("Rocket", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Hello, world!");
	ImGui::Text("Rocket Vulkan Render");
	ImGui::ColorEdit3("clear color", (float*)&clearColor);
	ImGui::End();
	ImGui::ShowDemoWindow();

	ImGui::EndFrame();
	//RK_CORE_INFO("EndFrame Info: {}, {}", context->FrameCount, context->FrameCountPlatformEnded);

	ImGui::Render();
	//RK_CORE_INFO("Render Info: {}, {}", context->FrameCount, context->FrameCountPlatformEnded);

	// Update and Render additional Platform Windows
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
	//RK_CORE_INFO("UpdatePlatformWindows Info: {}, {}", context->FrameCount, context->FrameCountPlatformEnded);
}

void VulkanUI::Draw(VkCommandBuffer cmdBuffer)
{
	ImDrawData* imDrawData = ImGui::GetDrawData();
	ImGui_ImplVulkan_RenderDrawData(imDrawData, cmdBuffer);
}

void VulkanUI::PostAction()
{
	// Update and Render additional Platform Windows
	//ImGuiContext* context = ImGui::GetCurrentContext();
	//ImGuiIO& io = ImGui::GetIO();
	//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	//{
	//	ImGui::UpdatePlatformWindows();
	//	ImGui::RenderPlatformWindowsDefault();
	//}
	//RK_CORE_INFO("UpdatePlatformWindows Info: {}, {}, {}", context->FrameCount, context->FrameCountPlatformEnded);
}
