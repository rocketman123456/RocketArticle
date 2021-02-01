#pragma once
#include "Interface/IPipelineStateManager.h"

#include <map>

namespace Rocket {
    Interface PipelineStateManager : inheritance IPipelineStateManager
    {
    public:
        PipelineStateManager() = default;
        virtual ~PipelineStateManager() = default;

        int Initialize() override;
        void Finalize() override;
        void Tick(Timestep ts) override {}

        bool RegisterPipelineState(PipelineState& pipelineState) override;
        void UnregisterPipelineState(PipelineState& pipelineState) override;
        void Clear() override;

        const Ref<PipelineState> GetPipelineState(const std::string& name) const final;

    protected:
        virtual bool InitializePipelineState(PipelineState** ppPipelineState) { return true; }
        virtual void DestroyPipelineState(PipelineState& pipelineState) {}

    protected:
        Map<std::string, Ref<PipelineState>> m_pipelineStates;
    };

    PipelineStateManager* GetPipelineStateManager();
    extern PipelineStateManager* g_PipelineStateManager;
}
