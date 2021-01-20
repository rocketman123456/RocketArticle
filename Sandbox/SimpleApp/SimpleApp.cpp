#include "SimpleApp.h"
#include "Module/WindowManager.h"
#include "Module/GraphicsManager.h"
#include "Module/ProcessManager.h"
#include "Module/EventManager.h"

namespace Rocket
{
    Application* g_Application;
    WindowManager* g_WindowManager;
    GraphicsManager* g_GraphicsManager;
    ProcessManager* g_ProcessManager;
    EventManager* g_EventManager;
    
    Application* CreateApplication()
    {
        g_Application = new SimpleApp();
        return g_Application;
    }

    void SimpleApp::PreInitializeModule()
    {
        g_WindowManager = GetWindowManager();
        g_GraphicsManager = GetGraphicsManager();
        g_ProcessManager = GetProcessManager();
        g_EventManager = GetEventManager();

        PushModule(g_WindowManager);
        PushModule(g_GraphicsManager);
        PushModule(g_ProcessManager);
        PushModule(g_EventManager);
    }

    void SimpleApp::PostInitializeModule()
    {
    }

    void SimpleApp::PreInitialize()
    {
    }

    void SimpleApp::PostInitialize()
    {
    }
}