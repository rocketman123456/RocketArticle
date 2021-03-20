#pragma once
#include "Module/PipelineStateManager.h"
#include "Vulkan/Render/VulkanFunction.h"

namespace Rocket
{
    struct VulkanPipelineState : public PipelineState
    {
        VulkanPipelineState(PipelineState& rhs) : PipelineState(rhs) {}
        VulkanPipelineState(PipelineState&& rhs) : PipelineState(std::move(rhs)) {}

        VkDevice device_handle;
        VkRenderPass render_pass;
        VkDescriptorSetLayout descriptor_set_layout;
        VkPipelineLayout pipeline_layout;
        VkPipeline graphics_pipeline;
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