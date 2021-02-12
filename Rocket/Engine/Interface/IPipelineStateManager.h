#pragma once
#include "Core/Core.h"
#include "Interface/IRuntimeModule.h"
#include "Module/MemoryManager.h"
#include "Render/DrawBasic/BufferLayout.h"
#include "Render/DrawBasic/FrameBuffer.h"
#include "Render/DrawBasic/VertexArray.h"
#include "Render/DrawBasic/VertexBuffer.h"
#include "Render/DrawBasic/IndexBuffer.h"
#include "Render/DrawBasic/Shader.h"

namespace Rocket
{
    ENUM(PIPELINE_TYPE) { GRAPHIC, COMPUTE };
    ENUM(PIPELINE_TARGET) { INVALID, PLANAR, SPATIAL };
    ENUM(DEPTH_TEST_MODE) { NONE, LARGE, LARGE_EQUAL, EQUAL, LESS_EQUAL, LESS, NOT_EQUAL, NEVER, ALWAYS };
    ENUM(BLENDER_MODE) { NONE, ONE_MINUS_SRC_ALPHA, ONE_MINUS_DST_ALPHA };
    ENUM(STENCIL_TEST_MODE) { NONE };
    ENUM(CULL_FACE_MODE) { NONE, FRONT, BACK };
    ENUM(PIXEL_FORMAT) { INVALID, BGRA8UNORM };
    ENUM(PIPELINE_FLAG) { NONE, SHADOW, DEBUG_DRAW };
    ENUM(A2V_TYPES) { A2V_TYPES_NONE, A2V_TYPES_FULL, A2V_TYPES_SIMPLE, A2V_TYPES_POS_ONLY, A2V_TYPES_CUBE };
    ENUM(RENDER_TARGET) { NONE, RENDER_FRAMEBUFFER };
    ENUM(RENDER_TYPE) { STATIC, DYNAMIC };

    struct PipelineState
    {
        virtual ~PipelineState() = default;

        String            pipelineStateName;
        PIPELINE_TYPE     pipelineType{PIPELINE_TYPE::GRAPHIC};
        PIPELINE_TARGET   pipelineTarget{PIPELINE_TARGET::INVALID};

        String            vertexShaderName;
        String            pixelShaderName;
        String            computeShaderName;
        String            geometryShaderName;
        String            tessControlShaderName;
        String            tessEvaluateShaderName;
        String            meshShaderName;

        DEPTH_TEST_MODE   depthTestMode{DEPTH_TEST_MODE::ALWAYS};
        bool              depthWriteMode{true};
        BLENDER_MODE      blenderMode{BLENDER_MODE::NONE};
        STENCIL_TEST_MODE stencilTestMode{STENCIL_TEST_MODE::NONE};
        CULL_FACE_MODE    cullFaceMode{CULL_FACE_MODE::BACK};
        PIXEL_FORMAT      pixelFormat{PIXEL_FORMAT::BGRA8UNORM};
        uint32_t          sampleCount{1};

        A2V_TYPES         a2vType{A2V_TYPES::A2V_TYPES_NONE};
        PIPELINE_FLAG     flag;

        RENDER_TYPE       renderType{RENDER_TYPE::STATIC};
        RENDER_TARGET     renderTarget{RENDER_TARGET::NONE};
        String            renderTargetName;
        FramebufferSpec   frameBufferInfo;

        Ref<Shader>       shaderProgram;
    };

    Interface IPipelineStateManager : inheritance IRuntimeModule
    {
    public:
        virtual bool RegisterPipelineState(PipelineState & pipelineState) = 0;
        virtual void UnregisterPipelineState(PipelineState & pipelineState) = 0;
        virtual void Clear() = 0;

        [[nodiscard]] virtual const Ref<PipelineState> GetPipelineState(const String& name) const = 0;
    };
}