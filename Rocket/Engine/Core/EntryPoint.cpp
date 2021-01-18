#include "Core/EntryPoint.h"

int main(int argc, char **argv)
{
    Rocket::Log::Init();
    RK_CORE_WARN("Initialize Log");

    auto app = Rocket::CreateApplication();

    app->LoadConfig();

    app->PreInitializeModule();
    if (app->InitializeModule() != 0)
        return 1;
    app->PostInitializeModule();

    app->PreInitialize();
    if (app->Initialize() != 0)
        return 1;
    app->PostInitialize();

    std::chrono::steady_clock Clock;
    std::chrono::duration<float> Duration;
    std::chrono::time_point<std::chrono::steady_clock> CurrentTime;
    std::chrono::time_point<std::chrono::steady_clock> LastTime;

    CurrentTime = Clock.now();
    LastTime = CurrentTime;
    
    while (app->IsRunning())
    {
        LastTime = CurrentTime;
        CurrentTime = Clock.now();
        Duration = CurrentTime - LastTime;

        app->Tick(Duration.count());
        app->TickModule(Duration.count());
    }
    
    app->Finalize();
    app->FinalizeModule();
    delete app;

    return 0;
}