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
    # Render
    #   Dispatch
    Render/DispatchPass/BaseDispatchPass.h
    Render/DispatchPass/BaseDispatchPass.cpp
    Render/DispatchPass/BRDFGenerate.h
    Render/DispatchPass/BRDFGenerate.cpp
    Render/DispatchPass/SkyBoxGenerate.h
    Render/DispatchPass/SkyBoxGenerate.cpp
    #   DrawBasic
    Render/DrawBasic/BufferLayout.h
    Render/DrawBasic/FrameBuffer.h
    Render/DrawBasic/IndexBuffer.h
    Render/DrawBasic/Shader.h
    Render/DrawBasic/Texture.h
    Render/DrawBasic/Texture.cpp
    Render/DrawBasic/UI.h
    Render/DrawBasic/UniformBuffer.h
    Render/DrawBasic/VertexArray.h
    Render/DrawBasic/VertexBuffer.h
    #   DrawPass
    Render/DrawPass/BaseDrawPass.h
    Render/DrawPass/BaseDrawPass.cpp
    Render/DrawPass/ForwardGeometryPass.h
    #   DrawSubPass
    Render/DrawSubPass/BaseSubPass.h
    Render/DrawSubPass/Geometry2DSubPass.h
    Render/DrawSubPass/Geometry2DSubPass.cpp
    Render/DrawSubPass/GeometrySubPass.h
    Render/DrawSubPass/GeometrySubPass.cpp
    Render/DrawSubPass/GuiSubPass.h
    Render/DrawSubPass/GuiSubPass.cpp
    Render/DrawSubPass/SkyBoxSubPass.h
    Render/DrawSubPass/SkyBoxSubPass.cpp
    #   RayTrace
    Render/RayTrace/Ray.h
    # Scene
    Scene/Scene.h
    Scene/Scene.cpp
    Scene/SceneNode.h
    Scene/SceneNode.cpp
    Scene/SceneComponent.h
    Scene/SceneComponent.cpp
    Scene/SceneSerializer.h
    Scene/SceneSerializer.cpp
    #   Component
    Scene/Component/Camera.h
    Scene/Component/Transform.h
    Scene/Component/Transform.cpp
    Scene/Component/PlanarMesh.h
    Scene/Component/PlanarMesh.cpp
    Scene/Component/EditorCamera.h
    Scene/Component/EditorCamera.cpp
    Scene/Component/SceneCamera.h
    Scene/Component/SceneCamera.cpp
    Scene/Component/Model.h
    Scene/Component/Model.cpp
    Scene/Component/TagName.h
    # Utils
    Utils/GenerateName.h
    Utils/GenerateName.cpp
    Utils/Hashing.h
    Utils/Hashing.cpp
    Utils/KeyCode.h
    Utils/MouseCode.h
    Utils/ThreadPool.h
    Utils/ThreadSafeQueue.h
    Utils/Timer.h
    Utils/Timer.cpp
    Utils/Variant.h
    Utils/Variant.cpp
    Utils/YamlMath.h
)