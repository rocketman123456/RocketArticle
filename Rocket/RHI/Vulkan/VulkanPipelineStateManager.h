#pragma once
#include "Module/PipelineStateManager.h"
#include "Module/PipelineStateManager.h"

namespace Rocket
{
    struct VulkanPipelineState : public PipelineState
    {
        VulkanPipelineState(PipelineState& rhs) : PipelineState(rhs) {}
        VulkanPipelineState(PipelineState&& rhs) : PipelineState(std::move(rhs)) {}
    };

    class VulkanPipelineStateManager : public PipelineStateManager
    {
    public:
        RUNTIME_MODULE_TYPE(VulkanPipelineStateManager);
        VulkanPipelineStateManager() = default;
        virtual ~VulkanPipelineStateManager() = default;

    protected:
        bool InitializePipelineState(PipelineState** ppPipelineState) final;
        void DestroyPipelineState(PipelineState& pipelineState) final;
    };
}