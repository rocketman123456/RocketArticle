#include "Module/PipelineStateManager.h"

#define VS_BASIC_SOURCE_FILE "basic.vert"
#define PS_BASIC_SOURCE_FILE "basic.frag"
#define VS_DRAW2D_SOURCE_FILE "draw2d.vert"
#define PS_DRAW2D_SOURCE_FILE "draw2d.frag"
#define DEBUG_VS_SHADER_SOURCE_FILE "debug.vert"
#define DEBUG_PS_SHADER_SOURCE_FILE "debug.frag"

namespace Rocket
{
    bool PipelineStateManager::RegisterPipelineState(PipelineState& pipelineState)
    {
        PipelineState* pPipelineState;
        pPipelineState = &pipelineState;
        if (InitializePipelineState(&pPipelineState))
        {
            m_pipelineStates.emplace(pipelineState.pipelineStateName, pPipelineState);
            return true;
        }
        return false;
    }

    void PipelineStateManager::UnregisterPipelineState(PipelineState& pipelineState)
    {
        const auto& it = m_pipelineStates.find(pipelineState.pipelineStateName);
        if (it != m_pipelineStates.end())
        {
            DestroyPipelineState(*it->second);
        }
        m_pipelineStates.erase(it);
    }

    void PipelineStateManager::Clear()
    {
        for (auto it = m_pipelineStates.begin(); it != m_pipelineStates.end(); it++)
        {
            if (it != m_pipelineStates.end())
            {
                DestroyPipelineState(*it->second);
            }
        }
        m_pipelineStates.clear();

        RK_CORE_ASSERT(m_pipelineStates.empty(), "Pipeline State Manager Clear ERROR");
        RK_CORE_INFO("Pipeline State Manager Clear has been called. ");
    }

    const Ref<PipelineState> PipelineStateManager::GetPipelineState(const std::string& name) const
    {
        const auto& it = m_pipelineStates.find(name);
        if (it != m_pipelineStates.end())
        {
            return it->second;
        }
        else
        {
            RK_CORE_ASSERT(!m_pipelineStates.empty(), "Cannot Find Required Pipeline State");
            return m_pipelineStates.begin()->second;
        }
    }

    int PipelineStateManager::Initialize()
    {
        PipelineState pipelineState;
        pipelineState.pipelineStateName = "Basic";
        pipelineState.pipelineType = PIPELINE_TYPE::GRAPHIC;
        pipelineState.vertexShaderName = VS_BASIC_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_BASIC_SOURCE_FILE;
        pipelineState.depthTestMode = DEPTH_TEST_MODE::LESS_EQUAL;
        pipelineState.bDepthWrite = true;
        pipelineState.stencilTestMode = STENCIL_TEST_MODE::NONE;
        pipelineState.cullFaceMode = CULL_FACE_MODE::BACK;
        //pipelineState.sampleCount = g_pApp->GetConfiguration().msaaSamples;
        pipelineState.a2vType = A2V_TYPES::A2V_TYPES_FULL;
        pipelineState.flag = PIPELINE_FLAG::NONE;
        RegisterPipelineState(pipelineState);

        pipelineState.pipelineStateName = "Draw2D";
        pipelineState.pipelineType = PIPELINE_TYPE::GRAPHIC;
        pipelineState.vertexShaderName = VS_DRAW2D_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_DRAW2D_SOURCE_FILE;
        pipelineState.depthTestMode = DEPTH_TEST_MODE::LESS_EQUAL;
        pipelineState.bDepthWrite = true;
        pipelineState.stencilTestMode = STENCIL_TEST_MODE::NONE;
        pipelineState.cullFaceMode = CULL_FACE_MODE::BACK;
        //pipelineState.sampleCount = g_pApp->GetConfiguration().msaaSamples;
        pipelineState.a2vType = A2V_TYPES::A2V_TYPES_FULL;
        pipelineState.flag = PIPELINE_FLAG::NONE;
        RegisterPipelineState(pipelineState);

        pipelineState.pipelineStateName = "Debug Draw";
        pipelineState.vertexShaderName = DEBUG_VS_SHADER_SOURCE_FILE;
        pipelineState.pixelShaderName = DEBUG_PS_SHADER_SOURCE_FILE;
        RegisterPipelineState(pipelineState);

        RK_CORE_INFO("Pipeline State Manager Initialized. [{}]", m_pipelineStates.size());
        return 0;
    }

    void PipelineStateManager::Finalize() 
    { 
        Clear(); 
    }
}