message(STATUS "Add Engine")
add_library( RocketEngine
    # Common
    Common/Allocator.h
    Common/BlockAllocator.h
    Common/BlockAllocator.cpp
    Common/Buffer.h
    Common/CommandParser.h
    Common/ConfigLoader.h
    Common/GeomMath.h
    Common/Mallocator.h
    Common/Window.h
    # Core
    Core/Assert.h
    Core/Core.h
    Core/Config.h
    Core/Define.h
    Core/EntryPoint.h
    Core/EntryPoint.cpp
    Core/Instrumentor.h
    Core/Instrumentor.h
    Core/Log.h
    Core/Log.cpp
    Core/Template.h
    # Event
    Event/Event.h
    # Graph
    Graph/Graph.h
    Graph/Graph.cpp
    Graph/GraphNode.h
    # Interface
    Interface/IAllocator.h
    Interface/IApplication.h
    Interface/IDispatchPass.h
    Interface/IDrawPass.h
    Interface/IDrawSubPass.h
    Interface/IEvent.h
    Interface/IMemoryManager.h
    Interface/IPass.h
    Interface/IRuntimeModule.h
    Interface/ISubPass.h
    # Logic
    Logic/StateMachine.h
    Logic/StateMachine.cpp
    # Module
    Module/Application.h
    Module/Application.cpp
    Module/AssetLoader.h
    Module/AssetLoader.cpp
    Module/AudioManager.h
    Module/AudioManager.cpp
    Module/EventManager.h
    Module/EventManager.cpp
    Module/GraphicsManager.h
    Module/GraphicsManager.cpp
    Module/MemoryManager.h
    Module/MemoryManager.cpp
    Module/PipelineStateManager.h
    Module/PipelineStateManager.cpp
    Module/ProcessManager.h
    Module/ProcessManager.cpp
    Module/SceneManager.h
    Module/SceneManager.cpp
    Module/WindowManager.h
    Module/WindowManager.cpp
    # Process
    Process/Process.h
    Process/Process.cpp
    # TODO : add headers
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