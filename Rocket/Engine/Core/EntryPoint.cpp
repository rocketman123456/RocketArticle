#include "Core/EntryPoint.h"

using namespace Rocket;

int main(int argc, char **argv)
{
    PROFILE_ENTRY();
    PROFILE_SET_THREAD(Main);

    Log::Init();
    RK_CORE_WARN("Initialize Log");

    RK_PROFILE_BEGIN_SESSION("ReadComfig", "RocketProfile-Startup.json");
    Ref<CommandParser> Parser = Ref<CommandParser>(new CommandParser(argc, argv));
    RK_CORE_INFO("CommandParser : {0}", Parser->ToString());
    
    String command;
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
    RK_CORE_INFO("ConfigLoader : {0}", Loader->ToString());
    RK_PROFILE_END_SESSION();
    
    RK_PROFILE_BEGIN_SESSION("Initialize", "RocketProfile-Initialize.json");
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
    RK_PROFILE_END_SESSION();

    float CountTime = 0.0f;
    int32_t CountFrame = 0;

    ElapseTimer Timer;
    Timer.Start();

    RK_PROFILE_BEGIN_SESSION("RunLoop", "RocketProfile-RunLoop.json");
    while (app->IsRunning())
    {
        PROFILE_SCOPE_CPU(MainLoop, 0);

        double Duration = Timer.GetTickTime();

        CountFrame++;
        CountTime += Duration;

        if(CountTime >= 1000.0f)
        {
            //RK_TRACE("FPS : {}", CountFrame);
            CountFrame = 0;
            CountTime = 0.0f;
        }

	    PROFILE_BEGIN_CPU_SAMPLE(ApplicationUpdate, 0);
	    app->Tick(Duration);
	    PROFILE_END_CPU_SAMPLE();
    }
    RK_PROFILE_END_SESSION();
    
    RK_PROFILE_BEGIN_SESSION("Finalize", "RocketProfile-Finalize.json");
    app->Finalize();
    app->FinalizeModule();
    delete app;
    RK_PROFILE_END_SESSION();

    PROFILE_EXIT();
    return 0;
}