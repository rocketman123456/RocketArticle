#pragma once
#include "Core/Core.h"
#include "Interface/IRuntimeModule.h"
#include "Module/MemoryManager.h"
#include "Render/DrawBasic/BufferLayout.h"
#include "Render/DrawBasic/FrameBuffer.h"
#include "Render/DrawBasic/VertexArray.h"
#include "Render/DrawBasic/VertexBuffer.h"
#include "Render/DrawBasic/IndexBuffer.h"

namespace Rocket
{
    ENUM(DEPTH_TEST_MODE) { NONE, LARGE, LARGE_EQUAL, EQUAL, LESS_EQUAL, LESS, NOT_EQUAL, NEVER, ALWAYS };
    ENUM(STENCIL_TEST_MODE) { NONE };
    ENUM(CULL_FACE_MODE) { NONE, FRONT, BACK };
    ENUM(PIPELINE_TYPE) { GRAPHIC, COMPUTE };
    ENUM(PIPELINE_FLAG) { NONE, SHADOW, DEBUG_DRAW };
    ENUM(PIXEL_FORMAT) { INVALID, BGRA8UNORM };
    ENUM(RENDER_FORMAT) { INVALID, PLANAR, SPATIAL };
    ENUM(A2V_TYPES) { A2V_TYPES_NONE, A2V_TYPES_FULL, A2V_TYPES_SIMPLE, A2V_TYPES_POS_ONLY, A2V_TYPES_CUBE };

    struct PipelineState
    {
        virtual ~PipelineState() = default;

        String              pipelineStateName;
        PIPELINE_TYPE       pipelineType{PIPELINE_TYPE::GRAPHIC};

        String              vertexShaderName;
        String              pixelShaderName;
        String              computeShaderName;
        String              geometryShaderName;
        String              tessControlShaderName;
        String              tessEvaluateShaderName;
        String              meshShaderName;

        DEPTH_TEST_MODE     depthTestMode{DEPTH_TEST_MODE::ALWAYS};
        bool                bDepthWrite{true};
        STENCIL_TEST_MODE   stencilTestMode{STENCIL_TEST_MODE::NONE};
        CULL_FACE_MODE      cullFaceMode{CULL_FACE_MODE::BACK};
        PIXEL_FORMAT        pixelFormat{PIXEL_FORMAT::BGRA8UNORM};
        uint32_t            sampleCount{1};

        A2V_TYPES           a2vType{A2V_TYPES::A2V_TYPES_NONE};
        PIPELINE_FLAG       flag;
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