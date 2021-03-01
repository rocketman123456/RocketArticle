message(STATUS "Add Module RHI Vulkan")

add_library( RendererVulkan
    VulkanPipelineStateManager.cpp
    VulkanGraphicsManager.cpp
    "Render/VulkanShader.cpp"
    "Render/VulkanBuffer.cpp"
    "Render/VulkanTexture.cpp"
    "Render/VulkanFunction.cpp"
    "Render/VulkanDevice.cpp"
    "Render/VulkanSwapChain.cpp"
    "Render/VulkanPipeline.cpp"
    "Render/VulkanFrameBuffer.cpp"
    "Render/VulkanUI.cpp"
)