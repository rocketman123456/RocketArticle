#include "Module/Application.h"

namespace Rocket
{
    Application* Application::s_Instance = nullptr;

    void Application::LoadConfig()
    {
        std::string config_file;
#if defined(PLATFORM_LINUX)
        config_file = ProjectSourceDir + "/Config/setting-linux.yaml";
#elif defined(PLATFORM_WINDOWS)
        config_file = ProjectSourceDir + "/Config/setting-windows.yaml";
#elif defined(PLATFORM_APPLE)
        config_file = ProjectSourceDir + "/Config/setting-mac.yaml";
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
            module->Finalize();
            delete module;
            module = nullptr;
        }
        m_Modules.clear();
    }

    int Application::Initialize()
    {
        m_CurrentTime = m_Clock.now();
        m_LastTime = m_CurrentTime;

        return 0;
    }

    void Application::Finalize()
    {
    }

    void Application::PushModule(IRuntimeModule *module)
    {
        m_Modules.emplace_back(module);
    }

    void Application::Tick()
    {
        // Calculate Delta Time
        m_LastTime = m_CurrentTime;
        m_CurrentTime = m_Clock.now();
        m_Duration = m_CurrentTime - m_LastTime;
    }

    void Application::TickModule()
    {
        for (auto &module : m_Modules)
        {
            module->Tick(Timestep(m_Duration.count()));
        }
    }

    bool Application::OnEvent(IEvent& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(RK_BIND_EVENT_FN_CLASS(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(RK_BIND_EVENT_FN_CLASS(Application::OnWindowResize));
        return event.Handled;
    }

    bool Application::OnWindowClose(WindowCloseEvent &e)
    {
        SetRunningState(false);
        e.Handled = true;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent &e)
    {
        if (e.GetWidth() == 0 || e.GetHeight() == 0)
        {
            m_Minimized = true;
            return false;
        }
        m_Minimized = false;
        return false;
    }
}