message(STATUS "Add Engine")
add_library( RocketEngine
    # Core
    Core/EntryPoint.cpp
    Core/Log.cpp
)
target_link_libraries( RocketEngine
    spdlog::spdlog_header_only
)