#include "OpenGL/OpenGLPipelineStateManager.h"
#include "Module/MemoryManager.h"

#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <utility>

using namespace Rocket;

PipelineStateManager* Rocket::GetPipelineStateManager() { return new OpenGLPipelineStateManager(); }

using ShaderSourceList = Vec<std::pair<GLenum, String>>;

bool OpenGLPipelineStateManager::InitializePipelineState(PipelineState** ppPipelineState)
{
    OpenGLPipelineState* pnew_state =new OpenGLPipelineState(**ppPipelineState);

    ShaderSourceList list;

    if (!(*ppPipelineState)->vertexShaderName.empty()) {
        list.emplace_back(GL_VERTEX_SHADER, (*ppPipelineState)->vertexShaderName);
    }

    if (!(*ppPipelineState)->pixelShaderName.empty()) {
        list.emplace_back(GL_FRAGMENT_SHADER, (*ppPipelineState)->pixelShaderName);
    }

    if (!(*ppPipelineState)->geometryShaderName.empty()) {
        list.emplace_back(GL_GEOMETRY_SHADER, (*ppPipelineState)->geometryShaderName);
    }

    if (!(*ppPipelineState)->computeShaderName.empty()) {
        list.emplace_back(GL_COMPUTE_SHADER, (*ppPipelineState)->computeShaderName);
    }

    if (!(*ppPipelineState)->tessControlShaderName.empty()) {
        list.emplace_back(GL_TESS_CONTROL_SHADER, (*ppPipelineState)->tessControlShaderName);
    }

    if (!(*ppPipelineState)->tessEvaluateShaderName.empty()) {
        list.emplace_back(GL_TESS_EVALUATION_SHADER, (*ppPipelineState)->tessEvaluateShaderName);
    }

    auto name = pnew_state->pipelineStateName + " Shader";
    pnew_state->shaderProgram = CreateRef<OpenGLShader>(name);
    bool result = pnew_state->shaderProgram->Initialize(list);
    *ppPipelineState = pnew_state;
        
    RK_GRAPHICS_INFO("Initialize Pipeline {} : {}", pnew_state->pipelineStateName, (*pnew_state->shaderProgram));

    return result;
}

void OpenGLPipelineStateManager::DestroyPipelineState(PipelineState& pipelineState)
{
    OpenGLPipelineState* pPipelineState = dynamic_cast<OpenGLPipelineState*>(&pipelineState);
    pPipelineState->shaderProgram->Finalize();
}
