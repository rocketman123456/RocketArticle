#include "Core/EntryPoint.h"
#include "Utils/Timer.h"

int main(int argc, char **argv)
{
    PROFILE_ENTRY();
    PROFILE_SET_THREAD(Main);

    Rocket::g_GlobalTimer = new Rocket::ProfilerTimer;
    Rocket::g_GlobalTimer->InitTime();

    Rocket::Log::Init();
    RK_CORE_WARN("Initialize Log");

    auto app = Rocket::CreateApplication();

    app->LoadConfig();

    app->PreInitializeModule();
    if (app->InitializeModule() != 0)
    {
        RK_CORE_ERROR("Application InitializeModule Failed");
	    return 1;
    }
    app->PostInitializeModule();

    app->PreInitialize();
    if (app->Initialize() != 0)
    {
        RK_CORE_ERROR("Application Initialize Failed");
	    return 1;
    }
    app->PostInitialize();

    std::chrono::steady_clock Clock;
    std::chrono::duration<float> Duration;
    std::chrono::time_point<std::chrono::steady_clock> CurrentTime;
    std::chrono::time_point<std::chrono::steady_clock> LastTime;

    CurrentTime = Clock.now();
    LastTime = CurrentTime;
    
    while (app->IsRunning())
    {
        PROFILE_SCOPE_CPU(MainLoop, 0);

        Rocket::g_GlobalTimer->MarkTimeThisTick();

        LastTime = CurrentTime;
        CurrentTime = Clock.now();
        Duration = CurrentTime - LastTime;

	    PROFILE_BEGIN_CPU_SAMPLE(ApplicationUpdate, 0);
	    app->Tick(Duration.count());
	    PROFILE_END_CPU_SAMPLE();

	    PROFILE_BEGIN_CPU_SAMPLE(ModuleUpdate, 0);
	    app->TickModule(Duration.count());
	    PROFILE_END_CPU_SAMPLE();
    }
    
    app->Finalize();
    app->FinalizeModule();
    delete app;

    PROFILE_EXIT();
    return 0;
}