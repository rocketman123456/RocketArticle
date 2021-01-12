#include "SimpleApp.h"
#include "Module/WindowManager.h"

namespace Rocket
{
    Application* CreateApplication()
    {
        return new SimpleApp();
    }

    void SimpleApp::PreInitializeModule()
    {
        PushModule(new WindowManager());
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