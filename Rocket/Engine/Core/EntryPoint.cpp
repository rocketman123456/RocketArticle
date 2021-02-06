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

    float CountTime = 0.0f;
    int32_t CountFrame = 0;

    ElapseTimer Timer;
    Timer.Start();

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
    
    app->Finalize();
    app->FinalizeModule();
    delete app;

    PROFILE_EXIT();
    return 0;
}