message(STATUS "Add Platform GLFWWindow")
add_library( PlatformWindow
    Common/WindowImplement.cpp
)
target_link_libraries( PlatformWindow PRIVATE
    spdlog::spdlog_header_only
)