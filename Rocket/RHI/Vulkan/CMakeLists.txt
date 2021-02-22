message(STATUS "Add Module RHI Vulkan")

add_library( RendererVulkan
    VulkanPipelineStateManager.cpp
    VulkanGraphicsManager.cpp
    VulkanShader.cpp
    VulkanBuffer.cpp
    VulkanTexture.cpp
    VulkanFunction.cpp
    VulkanDevice.cpp
    VulkanSwapChain.cpp
)