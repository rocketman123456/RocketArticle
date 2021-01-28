#include "Module/Application.h"
#include "Utils/Timer.h"

namespace Rocket
{
    Application* Application::s_Instance = nullptr;

    void Application::LoadConfig(const Ref<ConfigLoader>& config)
    {
        m_Config = config;
        m_AssetPath = config->GetAssetPath();
        RK_CORE_INFO("Asset Path {0}", m_AssetPath);
    }

    int Application::InitializeModule()
    {
        Rocket::g_GlobalTimer = new Rocket::ElapseTimer();
        Rocket::g_GlobalTimer->InitTime();

        int ret = 0;
        for (auto iter = m_Modules.begin(); iter != m_Modules.end(); iter++)
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
        for (auto iter = m_Modules.rbegin(); iter != m_Modules.rend(); iter++)
        {
            RK_CORE_INFO("Finalize Module {0}", (*iter)->GetName());
            (*iter)->Finalize();
            delete (*iter);
            (*iter) = nullptr;
        }
        m_Modules.clear();

        delete Rocket::g_GlobalTimer;
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
        m_Modules.emplace_back(module);
    }

    void Application::Tick(Timestep ts)
    {
        Rocket::g_GlobalTimer->MarkTimeThisTick();
    }

    void Application::TickModule(Timestep ts)
    {
        for (auto &module : m_Modules)
        {
            module->Tick(ts);
        }
    }

    bool Application::OnWindowClose(EventPtr& e)
    {
        RK_CORE_TRACE("Application::OnWindowClose");
        SetRunningState(false);
        e->Handled = true;
        return true;
    }

    bool Application::OnWindowResize(EventPtr& e)
    {
        RK_CORE_TRACE("Application::OnWindowResize");
        auto event = static_cast<WindowResizeEvent*>(e.get());
        if (event->GetWidth() == 0 || event->GetHeight() == 0)
        {
            m_Minimized = true;
            return false;
        }
        m_Minimized = false;
        return false;
    }
}