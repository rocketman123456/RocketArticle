#include "Vulkan/VulkanPipelineStateManager.h"
#include "Vulkan/VulkanShader.h"
#include "Module/MemoryManager.h"

#include <shaderc/shaderc.hpp>

using namespace Rocket;

PipelineStateManager* Rocket::GetPipelineStateManager() { return new VulkanPipelineStateManager(); }

bool VulkanPipelineStateManager::InitializePipelineState(PipelineState** ppPipelineState)
{
    VulkanPipelineState* pnew_state =new VulkanPipelineState(**ppPipelineState);

    ShaderSourceList list;

    if (!(*ppPipelineState)->vertexShaderName.empty()) {
        list.emplace_back(shaderc_glsl_vertex_shader, (*ppPipelineState)->vertexShaderName);
    }

    if (!(*ppPipelineState)->pixelShaderName.empty()) {
        list.emplace_back(shaderc_glsl_fragment_shader, (*ppPipelineState)->pixelShaderName);
    }

    if (!(*ppPipelineState)->geometryShaderName.empty()) {
        list.emplace_back(shaderc_glsl_geometry_shader, (*ppPipelineState)->geometryShaderName);
    }

    if (!(*ppPipelineState)->computeShaderName.empty()) {
        list.emplace_back(shaderc_glsl_compute_shader, (*ppPipelineState)->computeShaderName);
    }

    if (!(*ppPipelineState)->tessControlShaderName.empty()) {
        list.emplace_back(shaderc_glsl_tess_control_shader, (*ppPipelineState)->tessControlShaderName);
    }

    if (!(*ppPipelineState)->tessEvaluateShaderName.empty()) {
        list.emplace_back(shaderc_glsl_tess_evaluation_shader, (*ppPipelineState)->tessEvaluateShaderName);
    }

    auto name = pnew_state->pipelineStateName + " Shader";
    pnew_state->shaderProgram = CreateRef<VulkanShader>(name);
    bool result = pnew_state->shaderProgram->Initialize(list);
    *ppPipelineState = pnew_state;

	return false;
}

void VulkanPipelineStateManager::DestroyPipelineState(PipelineState& pipelineState)
{
    VulkanPipelineState* pPipelineState = dynamic_cast<VulkanPipelineState*>(&pipelineState);
    pPipelineState->shaderProgram->Finalize();
}