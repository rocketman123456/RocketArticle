#include "SimpleApp.h"
#include "Module/AssetLoader.h"
#include "Module/WindowManager.h"
#include "Module/GraphicsManager.h"
#include "Module/PipelineStateManager.h"
#include "Module/ProcessManager.h"
#include "Module/EventManager.h"
#include "Module/SceneManager.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Component.h"

namespace Rocket
{
    Application* g_Application;
    AssetLoader* g_AssetLoader;
    WindowManager* g_WindowManager;
    PipelineStateManager* g_PipelineStateManager;
    GraphicsManager* g_GraphicsManager;
    ProcessManager* g_ProcessManager;
    EventManager* g_EventManager;
    SceneManager* g_SceneManager;
    
    Application* CreateApplication()
    {
        g_Application = new SimpleApp();
        return g_Application;
    }

    void SimpleApp::PreInitializeModule()
    {
        g_AssetLoader = GetAssetLoader();
        g_WindowManager = GetWindowManager();
        g_GraphicsManager = GetGraphicsManager();
        g_PipelineStateManager = GetPipelineStateManager();
        g_SceneManager = GetSceneManager();
        g_ProcessManager = GetProcessManager();
        g_EventManager = GetEventManager();

        PushModule(g_AssetLoader);
        PushModule(g_WindowManager);
        PushModule(g_GraphicsManager);
        PushModule(g_PipelineStateManager);
        PushModule(g_SceneManager);
        PushModule(g_ProcessManager);
        PushModule(g_EventManager); // Make Event Manager Last to Register Listener
    }

    void SimpleApp::PostInitializeModule()
    {
        g_EventManager->AddListener(
            REGISTER_DELEGATE_CLASS(Application::OnWindowClose, *g_Application), 
            EventHashTable::HashString("window_close"));
        g_EventManager->AddListener(
            REGISTER_DELEGATE_CLASS(GraphicsManager::OnWindowResize, *g_GraphicsManager), 
            EventHashTable::HashString("window_resize"));
    }

    void SimpleApp::PreInitialize()
    {
        Ref<Scene> scene = Ref<Scene>(new Scene());

        auto square = scene->CreateEntity("Green Square");
        square.AddComponent<SpriteRendererComponent>(Vector4f({ 0.0f, 1.0f, 0.0f, 1.0f }));

        auto redSquare = scene->CreateEntity("Red Square");
        redSquare.AddComponent<SpriteRendererComponent>(Vector4f({ 1.0f, 0.0f, 0.0f, 1.0f }));

        auto camera = scene->CreateEntity("Camera A");
        auto com = camera.AddComponent<CameraComponent>();

        g_SceneManager->AddScene("first scene", scene);
        g_SceneManager->SetActiveScene("first scene");
        scene->SetPrimaryCamera(&com.Camera);
    }

    void SimpleApp::PostInitialize()
    {
    }
}