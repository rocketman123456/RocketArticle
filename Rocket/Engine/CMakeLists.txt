message(STATUS "Add Engine")
add_library( RocketEngine
    # Common
    Common/BlockAllocator.cpp
    # Core
    Core/EntryPoint.cpp
    Core/Log.cpp
    Core/Instrumentor.h
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
    #   Dispatch
    Render/DispatchPass/BaseDispatchPass.cpp
    Render/DispatchPass/BRDFGenerate.cpp
    Render/DispatchPass/SkyBoxGenerate.cpp
    #   DrawBasic
    Render/DrawBasic/Texture.cpp
    #   DrawPass
    Render/DrawPass/BaseDrawPass.cpp
    #   DrawSubPass
    Render/DrawSubPass/Geometry2DSubPass.cpp
    Render/DrawSubPass/GeometrySubPass.cpp
    Render/DrawSubPass/GuiSubPass.cpp
    Render/DrawSubPass/SkyBoxSubPass.cpp
    # Scene
    Scene/Scene.cpp
    Scene/SceneNode.cpp
    Scene/SceneComponent.cpp
    Scene/SceneSerializer.cpp
    #   Component
    Scene/Component/Transform.cpp
    Scene/Component/PlanarMesh.cpp
    Scene/Component/EditorCamera.cpp
    Scene/Component/SceneCamera.cpp
    Scene/Component/Model.cpp
    # Utils
    Utils/GenerateName.cpp
    Utils/Hashing.cpp
    Utils/Timer.cpp
    Utils/Variant.cpp
)