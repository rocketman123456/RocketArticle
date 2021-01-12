#include "SimpleApp.h"
#include "Module/WindowManager.h"
#include "Module/GraphicsManager.h"

namespace Rocket
{
    Application* CreateApplication()
    {
        return new SimpleApp();
    }

    WindowManager* g_WindowManager;
    GraphicsManager* g_GraphicsManager;

    void SimpleApp::PreInitializeModule()
    {
        auto window_manager = GetWindowManager();
        auto graphics_manager = GetGraphicsManager();
        g_WindowManager = window_manager;
        g_GraphicsManager = graphics_manager;
        PushModule(window_manager);
        PushModule(graphics_manager);
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