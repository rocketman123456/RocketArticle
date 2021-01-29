message(STATUS "Add Engine")
add_library( RocketEngine
    # Common
    Common/Window.cpp
    # Core
    Core/EntryPoint.cpp
    Core/Log.cpp
    # Module
    Module/Application.cpp
    Module/AssetLoader.cpp
    Module/EventManager.cpp
    Module/GraphicsManager.cpp
    Module/MemoryManager.cpp
    Module/PipelineStateManager.cpp
    Module/ProcessManager.cpp
    Module/SceneManager.cpp
    Module/WindowManager.cpp
    # Process
    Process/Process.cpp
    # Render
    Render/DispatchPass/BaseDispatchPass.cpp
    Render/DrawPass/BaseDrawPass.cpp
    Render/DrawSubPass/Draw2DSubPass.cpp
    Render/DrawSubPass/GeometrySubPass.cpp
    Render/DrawSubPass/GuiSubPass.cpp
    # Scene
    Scene/Scene.cpp
    # Utils
    Utils/Hashing.cpp
    Utils/Timer.cpp
)
target_link_libraries( RocketEngine PRIVATE
    spdlog::spdlog_header_only
)