#pragma once


namespace Rocket
{
    

    Interface IPipelineStateManager : inheritance IRuntimeModule
    {
    public:
        virtual bool RegisterPipelineState(PipelineState & pipelineState) = 0;
        virtual void UnregisterPipelineState(PipelineState & pipelineState) = 0;
        virtual void Clear() = 0;

        [[nodiscard]] virtual const Ref<PipelineState> GetPipelineState(const String& name) const = 0;
    };
}