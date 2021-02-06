#include "SimpleApp.h"
#include "Module/MemoryManager.h"
#include "Module/AssetLoader.h"
#include "Module/WindowManager.h"
#include "Module/GraphicsManager.h"
#include "Module/PipelineStateManager.h"
#include "Module/ProcessManager.h"
#include "Module/EventManager.h"
#include "Module/SceneManager.h"

#include "Scene/Scene.h"

namespace Rocket
{
    Application* g_Application;
    MemoryManager* g_MemoryManager;
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
        g_MemoryManager = GetMemoryManager();
        g_AssetLoader = GetAssetLoader();
        g_ProcessManager = GetProcessManager();
        g_SceneManager = GetSceneManager();
        g_WindowManager = GetWindowManager();
        g_GraphicsManager = GetGraphicsManager();
        g_PipelineStateManager = GetPipelineStateManager();
        g_EventManager = GetEventManager();

        PushModule(g_MemoryManager);
        PushModule(g_AssetLoader);
        PushModule(g_ProcessManager);
        PushModule(g_SceneManager);
        PushModule(g_WindowManager);
        PushModule(g_GraphicsManager);
        PushModule(g_PipelineStateManager);
        PushModule(g_EventManager);
    }

    void SimpleApp::PostInitializeModule()
    {
        bool ret = false;
        ret = g_EventManager->AddListener(
            REGISTER_DELEGATE_CLASS(Application::OnWindowClose, *g_Application), 
            EventHashTable::HashString("window_close"));
        ret = g_EventManager->AddListener(
            REGISTER_DELEGATE_CLASS(GraphicsManager::OnWindowResize, *g_GraphicsManager), 
            EventHashTable::HashString("window_resize"));
    }

    void SimpleApp::PreInitialize()
    {
        Ref<Scene> scene = CreateRef<Scene>("Test Scene");

        auto ret_1 = g_SceneManager->AddScene(scene);
        auto ret_2 = g_SceneManager->SetActiveScene("Test Scene");
    }

    void SimpleApp::PostInitialize()
    {
    }
}