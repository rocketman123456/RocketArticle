message(STATUS "Add Engine")
add_library( RocketEngine
    # Common
    Common/Window.cpp
    # Core
    Core/EntryPoint.cpp
    Core/Log.cpp
    # Module
    Module/Application.cpp
    Module/EventManager.cpp
    Module/GraphicsManager.cpp
    Module/MemoryManager.cpp
    Module/ProcessManager.cpp
    Module/SceneManager.cpp
    Module/WindowManager.cpp
    # Process
    Process/Process.cpp
    # Utils
    Utils/Timer.cpp
)
target_link_libraries( RocketEngine PRIVATE
    spdlog::spdlog_header_only
)