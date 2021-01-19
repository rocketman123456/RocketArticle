#include "Module/Application.h"

namespace Rocket
{
    Application* Application::s_Instance = nullptr;

    void Application::LoadConfig(const std::string& path)
    {
        std::string config_file;
#if defined(PLATFORM_LINUX)
        config_file = path + "/Config/setting-linux.yaml";
#elif defined(PLATFORM_WINDOWS)
        config_file = path + "/Config/setting-windows.yaml";
#elif defined(PLATFORM_APPLE)
        config_file = path + "/Config/setting-mac.yaml";
#endif
        m_Config = YAML::LoadFile(config_file);
        m_AssetPath = m_Config["asset_path"].as<std::string>();
        RK_CORE_INFO("Asset Path {0}", m_AssetPath);
    }

    int Application::InitializeModule()
    {
        int ret = 0;
        for (auto &module : m_Modules)
        {
            RK_CORE_INFO("Initialize Module {0}", module->GetName());
            if ((ret = module->Initialize()) != 0)
            {
                RK_CORE_ERROR("Failed. err = {0}, {1}", ret, module->GetName());
                return ret;
            }
        }
        return ret;
    }

    void Application::FinalizeModule()
    {
        for (auto &module : m_Modules)
        {
            RK_CORE_INFO("Finalize Module {0}", module->GetName());
            module->Finalize();
            delete module;
            module = nullptr;
        }
        m_Modules.clear();
    }

    int Application::Initialize()
    {
        return 0;
    }

    void Application::Finalize()
    {
    }

    void Application::PushModule(IRuntimeModule *module)
    {
        m_Modules.emplace_back(module);
    }

    void Application::Tick(Timestep ts)
    {
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