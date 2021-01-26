#include "Core/EntryPoint.h"

using namespace Rocket;

int main(int argc, char **argv)
{
    PROFILE_ENTRY();
    PROFILE_SET_THREAD(Main);

    Log::Init();
    RK_CORE_WARN("Initialize Log");

    Ref<CommandParser> Parser = Ref<CommandParser>(new CommandParser(argc, argv));
    RK_CORE_INFO("CommandParser : {0}", Parser->ToString());

    std::string command;
    if(argc > 1)
        command = Parser->GetCommand(1);
    else
        command = ProjectSourceDir;

    Ref<ConfigLoader> Loader  = Ref<ConfigLoader>(new ConfigLoader(command));
    int ret = Loader->Initialize();
    if(ret != 0)
    {
        RK_CORE_ERROR("Config Loader Initialize Failed");
        return 1;
    }

    auto app = CreateApplication();
    app->LoadConfig(Loader);

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