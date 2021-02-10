message(STATUS "Add Module RHI Vulkan")

add_library( RendererVulkan
    VulkanGraphicsManager.cpp
)
#target_link_libraries( RendererVulkan PRIVATE
#    spdlog::spdlog_header_only
#)