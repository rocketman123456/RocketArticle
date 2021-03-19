#include "Module/Application.h"
#include "Utils/Timer.h"

using namespace Rocket;

Application* Application::instance_ = nullptr;

void Application::LoadConfig(const Ref<ConfigLoader>& config)
{
    config_ = config;
    asset_path_ = config->GetAssetPath();
    RK_CORE_INFO("Asset Path {0}", asset_path_);
}

int Application::InitializeModule()
{
    int ret = 0;
    for (auto iter = modules_.begin(); iter != modules_.end(); iter++)
    {
        RK_CORE_INFO("Initialize Module {0}", (*iter)->GetName());
        if ((ret = (*iter)->Initialize()) != 0)
        {
            RK_CORE_ERROR("Failed. err = {0}, {1}", ret, (*iter)->GetName());
            return ret;
        }
    }
    return ret;
}

void Application::FinalizeModule()
{
    for (auto iter = modules_.rbegin(); iter != modules_.rend(); iter++)
    {
        RK_CORE_INFO("Finalize Module {0}", (*iter)->GetName());
        (*iter)->Finalize();
        delete (*iter);
        (*iter) = nullptr;
    }
    modules_.clear();
}

int Application::Initialize()
{
    RK_CORE_INFO("Application Initialize");
    return 0;
}

void Application::Finalize()
{
    RK_CORE_INFO("Application Finalize");
}

void Application::PushModule(IRuntimeModule* module)
{
    modules_.emplace_back(module);
}

void Application::Tick(Timestep ts)
{
	PROFILE_BEGIN_CPU_SAMPLE(ModuleUpdate, 0);
    for (auto &module : modules_)
    {
        module->Tick(ts);
    }
	PROFILE_END_CPU_SAMPLE();
}

bool Application::OnWindowClose(EventPtr& e)
{
    RK_CORE_TRACE("Application::OnWindowClose");
    SetRunningState(false);
    e->handled = false;
    return false;
}

bool Application::OnWindowResize(EventPtr& event)
{
    RK_CORE_TRACE("Application::OnWindowResize");
    if (event->GetInt32(1) == 0 || event->GetInt32(2) == 0)
    {
        minimized_ = true;
        return false;
    }
    minimized_ = false;
    return false;
}
