#pragma once
#include "Module/PipelineStateManager.h"
#include "OpenGL/OpenGLShader.h"

namespace Rocket
{
    struct OpenGLPipelineState : public PipelineState
    {
        Ref<OpenGLShader> shaderProgram;
        OpenGLPipelineState(PipelineState& rhs) : PipelineState(rhs) {}
        OpenGLPipelineState(PipelineState&& rhs) : PipelineState(std::move(rhs)) {}
    };

    class OpenGLPipelineStateManager : public PipelineStateManager
    {
    public:
        RUNTIME_MODULE_TYPE(OpenGLPipelineStateManager);
        OpenGLPipelineStateManager() = default;
        virtual ~OpenGLPipelineStateManager() = default;

    protected:
        bool InitializePipelineState(PipelineState** ppPipelineState) final;
        void DestroyPipelineState(PipelineState& pipelineState) final;
    };
}