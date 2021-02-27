#include "Vulkan/VulkanUI.h"
#include "Vulkan/VulkanFunction.h"
#include "Vulkan/VulkanShader.h"
#include "Module/AssetLoader.h"

// ImGui Implements
#include <backends/imgui_impl_glfw.cpp>
#include <backends/imgui_impl_vulkan.cpp>
#include <GLFW/glfw3.h>

using namespace Rocket;

void VulkanUI::Connect(
    Ref<VulkanDevice> device, 
    VkRenderPass renderPass, 
    VkQueue queue, 
    VkPipelineCache pipelineCache, 
    VkSampleCountFlagBits multiSampleCount)
{
    this->device = device;
    this->renderPass = renderPass;
    this->queue = queue;
    this->pipelineCache = pipelineCache;
    this->multiSampleCount = multiSampleCount;
}

void VulkanUI::Initialize()
{
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport / Platform Windows
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
	//style.FrameBorderSize = 0.0f;
	//style.WindowBorderSize = 0.0f;

	// Load Fonts
	io.Fonts->AddFontDefault();
    unsigned char* fontData;
    int texWidth, texHeight;
    ImFont* font;

    String asset_path = g_AssetLoader->GetAssetPath();
    String font_path = asset_path + "Fonts/Roboto-Medium.ttf";
    RK_GRAPHICS_TRACE(font_path);
   
    io.Fonts->AddFontFromFileTTF(font_path.c_str(), 16.0f);
    io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
	VkDeviceSize imageSize = texWidth * texHeight * 4;
    fontTexture.LoadFromBuffer(fontData, imageSize, VK_FORMAT_R8G8B8A8_UNORM, texWidth, texHeight, device, queue);    

    // Descriptor pool
	{
		std::vector<VkDescriptorPoolSize> poolSizes = {
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 }
		};
		VkDescriptorPoolCreateInfo descriptorPoolCI{};
		descriptorPoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCI.poolSizeCount = 1;
		descriptorPoolCI.pPoolSizes = poolSizes.data();
		descriptorPoolCI.maxSets = 1;
		VK_CHECK(vkCreateDescriptorPool(device->logicalDevice, &descriptorPoolCI, nullptr, &descriptorPool));
	}
	//RK_GRAPHICS_TRACE("Descriptor pool");

    // Descriptor set layout
	{
		VkDescriptorSetLayoutBinding setLayoutBinding{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr };
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCI{};
		descriptorSetLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCI.pBindings = &setLayoutBinding;
		descriptorSetLayoutCI.bindingCount = 1;
		VK_CHECK(vkCreateDescriptorSetLayout(device->logicalDevice, &descriptorSetLayoutCI, nullptr, &descriptorSetLayout));
	}
	//RK_GRAPHICS_TRACE("Descriptor set layout");

    // Descriptor set
	{
		VkDescriptorSetAllocateInfo descriptorSetAllocInfo{};
		descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocInfo.descriptorPool = descriptorPool;
		descriptorSetAllocInfo.pSetLayouts = &descriptorSetLayout;
		descriptorSetAllocInfo.descriptorSetCount = 1;
		VK_CHECK(vkAllocateDescriptorSets(device->logicalDevice, &descriptorSetAllocInfo, &descriptorSet));

		VkWriteDescriptorSet writeDescriptorSet{};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.dstSet = descriptorSet;
		writeDescriptorSet.dstBinding = 0;
		writeDescriptorSet.pImageInfo = &fontTexture.descriptor;

		vkUpdateDescriptorSets(device->logicalDevice, 1, &writeDescriptorSet, 0, nullptr);
	}
	//RK_GRAPHICS_TRACE("Descriptor set");

    // Pipeline layout
	{
		VkPushConstantRange pushConstantRange{ VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstBlock) };

		VkPipelineLayoutCreateInfo pipelineLayoutCI{};
		pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCI.pushConstantRangeCount = 1;
		pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;
		pipelineLayoutCI.setLayoutCount = 1;
		pipelineLayoutCI.pSetLayouts = &descriptorSetLayout;
		pipelineLayoutCI.pushConstantRangeCount = 1;
		pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;
		VK_CHECK(vkCreatePipelineLayout(device->logicalDevice, &pipelineLayoutCI, nullptr, &pipelineLayout));
	}
	//RK_GRAPHICS_TRACE("Pipeline layout");

    // Pipeline
	{
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCI{};
		inputAssemblyStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyStateCI.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		VkPipelineRasterizationStateCreateInfo rasterizationStateCI{};
		rasterizationStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationStateCI.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationStateCI.cullMode = VK_CULL_MODE_NONE;
		rasterizationStateCI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationStateCI.lineWidth = 1.0f;

		VkPipelineColorBlendAttachmentState blendAttachmentState{};
		blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		blendAttachmentState.blendEnable = VK_TRUE;
		blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlendStateCI{};
		colorBlendStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateCI.attachmentCount = 1;
		colorBlendStateCI.pAttachments = &blendAttachmentState;

		VkPipelineDepthStencilStateCreateInfo depthStencilStateCI{};
		depthStencilStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilStateCI.depthTestEnable = VK_FALSE;
		depthStencilStateCI.depthWriteEnable = VK_FALSE;
		depthStencilStateCI.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthStencilStateCI.front = depthStencilStateCI.back;
		depthStencilStateCI.back.compareOp = VK_COMPARE_OP_ALWAYS;

		VkPipelineViewportStateCreateInfo viewportStateCI{};
		viewportStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateCI.viewportCount = 1;
		viewportStateCI.scissorCount = 1;

		VkPipelineMultisampleStateCreateInfo multisampleStateCI{};
		multisampleStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleStateCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		if (multiSampleCount > VK_SAMPLE_COUNT_1_BIT) {
			multisampleStateCI.rasterizationSamples = multiSampleCount;
		}

		Vec<VkDynamicState> dynamicStateEnables = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicStateCI{};
		dynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCI.pDynamicStates = dynamicStateEnables.data();
		dynamicStateCI.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

		VkVertexInputBindingDescription vertexInputBinding = { 0, 20, VK_VERTEX_INPUT_RATE_VERTEX };
		Vec<VkVertexInputAttributeDescription> vertexInputAttributes = {
			{ 0, 0, VK_FORMAT_R32G32_SFLOAT, 0 },
			{ 1, 0, VK_FORMAT_R32G32_SFLOAT, sizeof(float) * 2 },
			{ 2, 0, VK_FORMAT_R8G8B8A8_UNORM, sizeof(float) * 4 },
		};
		VkPipelineVertexInputStateCreateInfo vertexInputStateCI{};
		vertexInputStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCI.vertexBindingDescriptionCount = 1;
		vertexInputStateCI.pVertexBindingDescriptions = &vertexInputBinding;
		vertexInputStateCI.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
		vertexInputStateCI.pVertexAttributeDescriptions = vertexInputAttributes.data();

		VkGraphicsPipelineCreateInfo pipelineCI{};
		pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCI.layout = pipelineLayout;
		pipelineCI.renderPass = renderPass;
		pipelineCI.pInputAssemblyState = &inputAssemblyStateCI;
		pipelineCI.pVertexInputState = &vertexInputStateCI;
		pipelineCI.pRasterizationState = &rasterizationStateCI;
		pipelineCI.pColorBlendState = &colorBlendStateCI;
		pipelineCI.pMultisampleState = &multisampleStateCI;
		pipelineCI.pViewportState = &viewportStateCI;
		pipelineCI.pDepthStencilState = &depthStencilStateCI;
		pipelineCI.pDynamicState = &dynamicStateCI;
		pipelineCI.layout = pipelineLayout;

		ShaderSourceList list;
		list.emplace_back(shaderc_glsl_vertex_shader, "ui.vert");
		list.emplace_back(shaderc_glsl_fragment_shader, "ui.frag");
		Ref<VulkanShader> shader = CreateRef<VulkanShader>("UI Shader");
		shader->SetDevice(device->logicalDevice);
		shader->Initialize(list);

		pipelineCI.stageCount = static_cast<uint32_t>(shader->GetShaderInfo().size());
		pipelineCI.pStages = shader->GetShaderInfo().data();
		VK_CHECK(vkCreateGraphicsPipelines(device->logicalDevice, pipelineCache, 1, &pipelineCI, nullptr, &pipeline));

		shader->Finalize();
	}
	//RK_GRAPHICS_TRACE("Pipeline");
}

void VulkanUI::Finalize()
{
	ImGui::DestroyContext();
	fontTexture.Finalize();
	if (canDraw)
	{
		vertexBuffer.Finalize();
		indexBuffer.Finalize();
	}
	vkDestroyPipeline(device->logicalDevice, pipeline, nullptr);
	vkDestroyPipelineLayout(device->logicalDevice, pipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(device->logicalDevice, descriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(device->logicalDevice, descriptorPool, nullptr);
}

void VulkanUI::UpdataOverlay(uint32_t width, uint32_t height)
{
	ImGuiIO& io = ImGui::GetIO();

	ImVec2 lastDisplaySize = io.DisplaySize;
	io.DisplaySize = ImVec2((float)width, (float)height);

	auto left = glfwGetMouseButton(windowHandle, GLFW_MOUSE_BUTTON_LEFT);
	auto right = glfwGetMouseButton(windowHandle, GLFW_MOUSE_BUTTON_RIGHT);

	double x, y;
	glfwGetCursorPos(windowHandle, &x, &y);

	io.MousePos = ImVec2(x, y);
	io.MouseDown[0] = left;
	io.MouseDown[1] = right;

	pushConstBlock.scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
	pushConstBlock.translate = glm::vec2(-1.0f);

	ImGui::NewFrame();

	ImGui::Begin("Rocket");
	ImGui::Text("Hello, world!");
	ImGui::ColorEdit3("clear color", (float*)&clearColor);
	ImGui::End();

	ImGui::Render();

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void VulkanUI::PrepareUI()
{
	canDraw = false;
	ImDrawData* imDrawData = ImGui::GetDrawData();

	if (imDrawData) {
		//RK_GRAPHICS_TRACE("GUI Vertex {}, Index {}", imDrawData->TotalVtxCount, imDrawData->TotalIdxCount);
		// First Call Of This Funtion will give 0 index and vertex count
		if (imDrawData->TotalVtxCount == 0 || imDrawData->TotalIdxCount == 0)
		{
			return;
		}

		// Check if ui buffers need to be recreated
		canDraw = true;

		VkDeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
		VkDeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

		bool updateBuffers =
			(vertexBuffer.buffer == VK_NULL_HANDLE) ||
			(vertexBuffer.count != imDrawData->TotalVtxCount) ||
			(indexBuffer.buffer == VK_NULL_HANDLE) ||
			(indexBuffer.count != imDrawData->TotalIdxCount);

		if (updateBuffers) {
			vkDeviceWaitIdle(device->logicalDevice);
			if (vertexBuffer.buffer) {
				vertexBuffer.Finalize();
			}
			vertexBuffer.Create(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, vertexBufferSize);
			vertexBuffer.count = imDrawData->TotalVtxCount;
			if (indexBuffer.buffer) {
				indexBuffer.Finalize();
			}
			indexBuffer.Create(device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, indexBufferSize);
			indexBuffer.count = imDrawData->TotalIdxCount;
		}

		// Upload data
		ImDrawVert* vtxDst = (ImDrawVert*)vertexBuffer.mapped;
		ImDrawIdx* idxDst = (ImDrawIdx*)indexBuffer.mapped;
		for (int n = 0; n < imDrawData->CmdListsCount; n++) {
			const ImDrawList* cmd_list = imDrawData->CmdLists[n];
			memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
			memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
			vtxDst += cmd_list->VtxBuffer.Size;
			idxDst += cmd_list->IdxBuffer.Size;
		}

		vertexBuffer.Flush();
		indexBuffer.Flush();
	}
}

void VulkanUI::Draw(VkCommandBuffer cmdBuffer)
{
	if (!canDraw)
		return;

	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

	const VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexBuffer.buffer, offsets);
	vkCmdBindIndexBuffer(cmdBuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

	vkCmdPushConstants(cmdBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VulkanUI::PushConstBlock), &pushConstBlock);

	ImDrawData* imDrawData = ImGui::GetDrawData();
	int32_t vertexOffset = 0;
	int32_t indexOffset = 0;
	for (int32_t j = 0; j < imDrawData->CmdListsCount; j++) {
		const ImDrawList* cmd_list = imDrawData->CmdLists[j];
		for (int32_t k = 0; k < cmd_list->CmdBuffer.Size; k++) {
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[k];
			VkRect2D scissorRect;
			scissorRect.offset.x = std::max((int32_t)(pcmd->ClipRect.x), 0);
			scissorRect.offset.y = std::max((int32_t)(pcmd->ClipRect.y), 0);
			scissorRect.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
			scissorRect.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y);
			vkCmdSetScissor(cmdBuffer, 0, 1, &scissorRect);
			vkCmdDrawIndexed(cmdBuffer, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
			indexOffset += pcmd->ElemCount;
		}
		vertexOffset += cmd_list->VtxBuffer.Size;
	}
}
