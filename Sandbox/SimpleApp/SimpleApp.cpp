#include "SimpleApp.h"
#include "Module/WindowManager.h"
#include "Module/GraphicsManager.h"
#include "Module/ProcessManager.h"

namespace Rocket
{
    Application* CreateApplication()
    {
        return new SimpleApp();
    }

    WindowManager* g_WindowManager;
    GraphicsManager* g_GraphicsManager;
    ProcessManager* g_ProcessManager;

    void SimpleApp::PreInitializeModule()
    {
        g_WindowManager = GetWindowManager();
        g_GraphicsManager = GetGraphicsManager();
        g_ProcessManager = GetProcessManager();

        PushModule(g_WindowManager);
        PushModule(g_GraphicsManager);
        PushModule(g_ProcessManager);
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