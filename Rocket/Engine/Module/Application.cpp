#include "Module/Application.h"

namespace Rocket
{
    Application* Application::s_Instance = nullptr;

    int Application::InitializeModule()
    {
        RK_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        int ret = 0;
        for (auto &module : m_Modules)
        {
            if ((ret = module->Initialize()) != 0)
            {
                RK_CORE_ERROR("Failed. err = {0}", ret);
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
        RK_PROFILE_FUNCTION();
        m_Modules.push_back(module);
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

    void Application::Close()
    {
        m_Running = false;
    }
}