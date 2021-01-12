#include "Core/EntryPoint.h"

int main(int argc, char **argv)
{
    Rocket::Log::Init();
    RK_CORE_WARN("Initialize Log");

    auto app = Rocket::CreateApplication();
    
    app->PreInitializeModule();
    if (app->InitializeModule() != 0)
        return 1;
    app->PostInitializeModule();

    app->PreInitialize();
    if (app->Initialize() != 0)
        return 1;
    app->PostInitialize();
    
    while (app->IsRunning())
    {
        app->Tick();
        app->TickModule();
    }
    
    app->Finalize();
    app->FinalizeModule();
    delete app;

    return 0;
}