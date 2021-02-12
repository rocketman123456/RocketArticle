#include "Vulkan/VulkanPipelineStateManager.h"

using namespace Rocket;

PipelineStateManager* Rocket::GetPipelineStateManager() { return new VulkanPipelineStateManager(); }

bool VulkanPipelineStateManager::InitializePipelineState(PipelineState** ppPipelineState)
{
	return false;
}

void VulkanPipelineStateManager::DestroyPipelineState(PipelineState& pipelineState)
{
    VulkanPipelineState* pPipelineState = dynamic_cast<VulkanPipelineState*>(&pipelineState);
    pPipelineState->shaderProgram->Finalize();
}