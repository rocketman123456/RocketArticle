#include "Core/EntryPoint.h"

using namespace Rocket;

int main(int argc, char **argv)
{
    PROFILE_ENTRY();
    PROFILE_SET_THREAD(Main);

    Log::Init();
    RK_CORE_WARN("Initialize Log");

    RK_PROFILE_BEGIN_SESSION("ReadComfig", "RocketProfile-Startup.json");
    Ref<CommandParser> parser = Ref<CommandParser>(new CommandParser(argc, argv));
    RK_CORE_INFO("CommandParser : {0}", parser->ToString());
    
    String command;
    if(argc > 1)
        command = parser->GetCommand(1);
    else
        command = ProjectSourceDir;

    Ref<ConfigLoader> loader  = Ref<ConfigLoader>(new ConfigLoader(command));
    
    int ret = loader->Initialize();
    if(ret != 0)
    {
        RK_CORE_ERROR("Config Loader Initialize Failed");
        return 1;
    }
    RK_CORE_INFO("ConfigLoader : {0}", loader->ToString());
    RK_PROFILE_END_SESSION();
    
    RK_PROFILE_BEGIN_SESSION("Initialize", "RocketProfile-Initialize.json");
    auto app = CreateApplication();
    app->LoadConfig(loader);

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

    float count_time = 0.0f;
    int32_t count_fps = 0;

    ElapseTimer timer;
    timer.Start();

    RK_PROFILE_BEGIN_SESSION("RunLoop", "RocketProfile-RunLoop.json");
    while (app->IsRunning())
    {
        PROFILE_SCOPE_CPU(MainLoop, 0);

        double duration = timer.GetTickTime();

        count_fps++;
        count_time += duration;

        if(count_time >= 1000.0f)
        {
            //RK_TRACE("FPS : {}", CountFrame);
            count_fps = 0;
            count_time = 0.0f;
        }

	    PROFILE_BEGIN_CPU_SAMPLE(ApplicationUpdate, 0);
	    app->Tick(duration);
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