message(STATUS "Add Platform GLFWWindow")
add_library( PlatformWindow
    WindowImplement.cpp
)
#target_link_libraries( PlatformWindow PRIVATE
#    spdlog::spdlog_header_only
#)