#include "Module/PipelineStateManager.h"
#include "Module/Application.h"
#include "Module/WindowManager.h"

#define VS_BASIC_SOURCE_FILE "basic.vert"
#define PS_BASIC_SOURCE_FILE "basic.frag"
#define VS_DRAW2D_SOURCE_FILE "draw2d.vert"
#define GS_DRAW2D_SOURCE_FILE "draw2d.geom"
#define PS_DRAW2D_SOURCE_FILE "draw2d.frag"
#define DEBUG_VS_SHADER_SOURCE_FILE "debug.vert"
#define DEBUG_PS_SHADER_SOURCE_FILE "debug.frag"
#define VS_SCREEN_SOURCE_FILE "screen.vert"
#define PS_SCREEN_SOURCE_FILE "screen.frag"
#define VS_PBR_SOURCE_FILE "pbr.vert"
#define PS_PBR_SOURCE_FILE "pbr_khr.frag"

using namespace Rocket;

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

    RK_GRAPHICS_ASSERT(m_pipelineStates.empty(), "Pipeline State Manager Clear ERROR");
    RK_GRAPHICS_INFO("Pipeline State Manager Clear has been called. ");
}

const Ref<PipelineState> PipelineStateManager::GetPipelineState(const String& name) const
{
    const auto& it = m_pipelineStates.find(name);
    if (it != m_pipelineStates.end())
    {
        return it->second;
    }
    else
    {
        RK_GRAPHICS_ASSERT(!m_pipelineStates.empty(), "Cannot Find Required Pipeline State");
        return nullptr;
    }
}

int PipelineStateManager::Initialize()
{
    auto& config = g_Application->GetConfig();
    auto& window = g_WindowManager->GetWindow();

    PipelineState pipelineState;

    // Draw2D
    {
        pipelineState.pipelineStateName = "Draw2D";
        pipelineState.pipelineType = PIPELINE_TYPE::GRAPHIC;
        pipelineState.pipelineTarget = PIPELINE_TARGET::PLANAR;
        pipelineState.vertexShaderName = VS_DRAW2D_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_DRAW2D_SOURCE_FILE;
        pipelineState.bufferLayout = {
            { ShaderDataType::Vec4f, "a_Position" },
            { ShaderDataType::Vec4f, "a_Color" },
            { ShaderDataType::Vec2f, "a_TexCoord" },
            { ShaderDataType::Float, "a_TexIndex" },
            { ShaderDataType::Float, "a_TilingFactor" },
        };
        pipelineState.uniformLayout = {
            {"PerFrameConstants", 0, 1, 1, 6},
            {"PerBatchConstants", 1, 1, 1, 6},
            {"u_Textures", 2, 16, 16, 1},
        };
        pipelineState.depthTestMode = DEPTH_TEST_MODE::LESS_EQUAL;
        pipelineState.depthWriteMode = true;
        pipelineState.blenderMode = BLENDER_MODE::ONE_MINUS_SRC_ALPHA;
        pipelineState.stencilTestMode = STENCIL_TEST_MODE::NONE;
        pipelineState.cullFaceMode = CULL_FACE_MODE::BACK;
        pipelineState.pixelFormat = PIXEL_FORMAT::BGRA8UNORM;
        pipelineState.sampleCount = config->GetConfigInfo<uint32_t>("Graphics", "msaa_sample_count");
        pipelineState.a2vType = A2V_TYPES::A2V_TYPES_FULL;
        pipelineState.flag = PIPELINE_FLAG::NONE;
        pipelineState.renderType = RENDER_TYPE::STATIC;
        pipelineState.renderTarget = RENDER_TARGET::NONE;
        //pipelineState.renderTarget = RENDER_TARGET::RENDER_FRAMEBUFFER;
        pipelineState.renderTargetName = "Draw2D Buffer";
        pipelineState.frameBufferInfo.ColorWidth = window->GetWidth();
        pipelineState.frameBufferInfo.ColorHeight = window->GetHeight();
        pipelineState.frameBufferInfo.ColorAttachment = {
            FRAME_TEXTURE_FORMAT::RGBA8
        };
        pipelineState.frameBufferInfo.DepthWidth = window->GetWidth();
        pipelineState.frameBufferInfo.DepthHeight = window->GetHeight();
        pipelineState.frameBufferInfo.DepthAttachment = FRAME_TEXTURE_FORMAT::DEPTH24;
        pipelineState.frameBufferInfo.Samples = config->GetConfigInfo<uint32_t>("Graphics", "msaa_sample_count");
        pipelineState.frameBufferInfo.SwapChainTarget = false;
        RegisterPipelineState(pipelineState);
    }
    
    // Screen
    {
        pipelineState.pipelineStateName = "Screen";
        pipelineState.pipelineType = PIPELINE_TYPE::GRAPHIC;
        pipelineState.pipelineTarget = PIPELINE_TARGET::PLANAR;
        pipelineState.vertexShaderName = VS_SCREEN_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_SCREEN_SOURCE_FILE;
        pipelineState.bufferLayout = {
            { ShaderDataType::Vec2f, "a_Position" },
            { ShaderDataType::Vec2f, "a_TexCoords" },
        };
        pipelineState.uniformLayout = {
            {"screenTexture", 0, 1, 16, 1}
        };
        pipelineState.depthTestMode = DEPTH_TEST_MODE::LESS_EQUAL;
        pipelineState.depthWriteMode = true;
        pipelineState.blenderMode = BLENDER_MODE::ONE_MINUS_SRC_ALPHA;
        pipelineState.stencilTestMode = STENCIL_TEST_MODE::NONE;
        pipelineState.cullFaceMode = CULL_FACE_MODE::BACK;
        pipelineState.pixelFormat = PIXEL_FORMAT::BGRA8UNORM;
        pipelineState.sampleCount = config->GetConfigInfo<uint32_t>("Graphics", "msaa_sample_count");
        pipelineState.a2vType = A2V_TYPES::A2V_TYPES_FULL;
        pipelineState.flag = PIPELINE_FLAG::NONE;
        pipelineState.renderType = RENDER_TYPE::STATIC;
        pipelineState.renderTarget = RENDER_TARGET::NONE;
        pipelineState.renderTargetName = "Screen Buffer";
        pipelineState.frameBufferInfo.ColorWidth = window->GetWidth();
        pipelineState.frameBufferInfo.ColorHeight = window->GetHeight();
        pipelineState.frameBufferInfo.ColorAttachment = {
            FRAME_TEXTURE_FORMAT::RGBA8
        };
        pipelineState.frameBufferInfo.DepthWidth = window->GetWidth();
        pipelineState.frameBufferInfo.DepthHeight = window->GetHeight();
        pipelineState.frameBufferInfo.DepthAttachment = FRAME_TEXTURE_FORMAT::DEPTH24;
        pipelineState.frameBufferInfo.Samples = config->GetConfigInfo<uint32_t>("Graphics", "msaa_sample_count");
        pipelineState.frameBufferInfo.SwapChainTarget = false;
        //RegisterPipelineState(pipelineState);
    }
    
    // PBR
    {
        pipelineState.pipelineStateName = "PBR";
        pipelineState.pipelineType = PIPELINE_TYPE::GRAPHIC;
        pipelineState.pipelineTarget = PIPELINE_TARGET::PLANAR;
        pipelineState.vertexShaderName = VS_PBR_SOURCE_FILE;
        pipelineState.pixelShaderName = PS_PBR_SOURCE_FILE;
        pipelineState.bufferLayout = {
            { ShaderDataType::Vec2f, "a_Position" },
            { ShaderDataType::Vec2f, "a_TexCoords" },
        };
        pipelineState.uniformLayout = {
            {"pbrTexture", 0, 1, 16, 1}
        };
        pipelineState.depthTestMode = DEPTH_TEST_MODE::LESS_EQUAL;
        pipelineState.depthWriteMode = true;
        pipelineState.blenderMode = BLENDER_MODE::ONE_MINUS_SRC_ALPHA;
        pipelineState.stencilTestMode = STENCIL_TEST_MODE::NONE;
        pipelineState.cullFaceMode = CULL_FACE_MODE::BACK;
        pipelineState.pixelFormat = PIXEL_FORMAT::BGRA8UNORM;
        pipelineState.sampleCount = config->GetConfigInfo<uint32_t>("Graphics", "msaa_sample_count");
        pipelineState.a2vType = A2V_TYPES::A2V_TYPES_FULL;
        pipelineState.flag = PIPELINE_FLAG::NONE;
        pipelineState.renderType = RENDER_TYPE::STATIC;
        pipelineState.renderTarget = RENDER_TARGET::NONE;
        pipelineState.renderTargetName = "PBR Buffer";
        pipelineState.frameBufferInfo.ColorWidth = window->GetWidth();
        pipelineState.frameBufferInfo.ColorHeight = window->GetHeight();
        pipelineState.frameBufferInfo.ColorAttachment = {
            FRAME_TEXTURE_FORMAT::RGBA8
        };
        pipelineState.frameBufferInfo.DepthWidth = window->GetWidth();
        pipelineState.frameBufferInfo.DepthHeight = window->GetHeight();
        pipelineState.frameBufferInfo.DepthAttachment = FRAME_TEXTURE_FORMAT::DEPTH24;
        pipelineState.frameBufferInfo.Samples = config->GetConfigInfo<uint32_t>("Graphics", "msaa_sample_count");
        pipelineState.frameBufferInfo.SwapChainTarget = false;
        //RegisterPipelineState(pipelineState);
    }
    
    RK_GRAPHICS_INFO("Pipeline State Manager Initialized. Add [{}] Pipelines", m_pipelineStates.size());
    return 0;
}

void PipelineStateManager::Finalize() 
{ 
    Clear(); 
}
