#pragma once
#include "Module/PipelineStateManager.h"
#include "Vulkan/Render/VulkanFunction.h"

namespace Rocket
{
    struct VulkanPipelineState : public PipelineState
    {
        VulkanPipelineState(PipelineState& rhs) : PipelineState(rhs) {}
        VulkanPipelineState(PipelineState&& rhs) : PipelineState(std::move(rhs)) {}

        VkDevice m_DeviceHandle;
        VkRenderPass m_RenderPass;
        VkDescriptorSetLayout m_DescriptorSetLayout;
        VkPipelineLayout m_PipelineLayout;
        VkPipeline m_GraphicsPipeline;
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