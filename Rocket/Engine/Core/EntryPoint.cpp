#include "Core/EntryPoint.h"
#include "Utils/Timer.h"

#ifdef RK_PROFILE
#include <Remotery.h>
#endif

int main(int argc, char **argv)
{
#ifdef RK_PROFILE
    Remotery *rmt;
	rmt_CreateGlobalInstance(&rmt);
#endif

#ifdef RK_PROFILE
    rmt_SetCurrentThreadName("Main");
#endif

    Rocket::g_GlobalTimer = new Rocket::ProfilerTimer;
    Rocket::g_GlobalTimer->InitTime();

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
#ifdef RK_PROFILE
        rmt_ScopedCPUSample(MainLoop, 0);
#endif

        Rocket::g_GlobalTimer->MarkTimeThisTick();

        LastTime = CurrentTime;
        CurrentTime = Clock.now();
        Duration = CurrentTime - LastTime;

#ifdef RK_PROFILE
        rmt_BeginCPUSample(ApplicationUpdate, 0);
#endif
        app->Tick(Duration.count());
#ifdef RK_PROFILE
        rmt_EndCPUSample();
#endif

#ifdef RK_PROFILE
        rmt_BeginCPUSample(ModuleUpdate, 0);
#endif
        app->TickModule(Duration.count());
#ifdef RK_PROFILE
        rmt_EndCPUSample();
#endif
    }
    
    app->Finalize();
    app->FinalizeModule();
    delete app;

    return 0;
}