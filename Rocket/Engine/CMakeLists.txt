message(STATUS "Add Engine")
add_library( RocketEngine
    # Common
    Common/BlockAllocator.cpp
    # Core
    Core/EntryPoint.cpp
    Core/Log.cpp
    # Graph
    Graph/Graph.cpp
    # Module
    Module/Application.cpp
    Module/AssetLoader.cpp
    Module/AudioManager.cpp
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
    Render/DispatchPass/BRDFPrepare.cpp
    Render/DrawBasic/Texture.cpp
    Render/DrawPass/BaseDrawPass.cpp
    Render/DrawSubPass/Geometry2DSubPass.cpp
    Render/DrawSubPass/GeometrySubPass.cpp
    Render/DrawSubPass/GuiSubPass.cpp
    Render/DrawSubPass/SkyBoxSubPass.cpp
    # Scene
    Scene/Component/Transform.cpp
    Scene/Component/PlanarMesh.cpp
    Scene/Component/EditorCamera.cpp
    Scene/Component/SceneCamera.cpp
    Scene/Scene.cpp
    Scene/SceneNode.cpp
    Scene/SceneComponent.cpp
    Scene/SceneSerializer.cpp
    # Utils
    Utils/GenerateName.cpp
    Utils/Hashing.cpp
    Utils/Timer.cpp
    Utils/Variant.cpp
)