#include "Core/EntryPoint.h"

int main(int argc, char **argv)
{
    PROFILE_ENTRY();
    PROFILE_SET_THREAD(Main);

    Rocket::Log::Init();
    RK_CORE_WARN("Initialize Log");

    Rocket::CommandParser Parser(argc, argv);
    RK_CORE_INFO("CommandParser : {0}", Parser.ToString());

    auto app = Rocket::CreateApplication();
    
    if(argc > 1)
    {
        auto command = Parser.GetCommand(1);
        app->LoadConfig(command);
    }
    else
    {
        app->LoadConfig(ProjectSourceDir);
    }

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

        LastTime = CurrentTime;
        CurrentTime = Clock.now();
        Duration = CurrentTime - LastTime;
        float dt = Duration.count();

	    PROFILE_BEGIN_CPU_SAMPLE(ApplicationUpdate, 0);
	    app->Tick(dt);
	    PROFILE_END_CPU_SAMPLE();

	    PROFILE_BEGIN_CPU_SAMPLE(ModuleUpdate, 0);
	    app->TickModule(dt);
	    PROFILE_END_CPU_SAMPLE();
    }
    
    app->Finalize();
    app->FinalizeModule();
    delete app;

    PROFILE_EXIT();
    return 0;
}