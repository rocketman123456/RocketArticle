#include "GEModule/Application.h"
#include "GEModule/WindowManager.h"

namespace Rocket
{
    Application *Application::s_Instance = nullptr;

    int Application::Initialize()
    {
        // Initialize Layers
        m_CurrentTime = m_Clock.now();
        m_LastTime = m_CurrentTime;

        return 0;
    }

    void Application::Finalize()
    {
        // Finalize Layers
    }

    int Application::InitializeModule()
    {
        // Initialize Modules
        int ret = 0;
        for (auto& module : m_Modules)
        {
            if ((ret = module->Initialize()) != 0) {
                RK_CORE_ERROR("Failed. err = {0}", ret);
                return ret;
            }
        }

        m_Window = g_WindowManager->GetWindow();
        Renderer::Init();
        return ret;
    }

    void Application::FinalizeModule()
    {
        // Finalize Modules
        for (auto& module : m_Modules)
        {
            module->Finalize();
            delete module;
        }

        Renderer::Shutdown();
    }

    void Application::PushModule(IRuntimeModule* module)
    {
        m_Modules.push_back(module);
    }

    void Application::OnEvent(Event &e)
    {
        // Send Events
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(RK_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(RK_BIND_EVENT_FN(Application::OnWindowResize));
    }

    void Application::Tick()
    {
        // Calculate Delta Time
        m_LastTime = m_CurrentTime;
        m_CurrentTime = m_Clock.now();
        m_Duration = m_CurrentTime - m_LastTime;
        // Poll Event Must Call in Main Thread
        m_Window->PollEvent();
    }

    void Application::TickModule()
    {
        for (auto& module : m_Modules)
        {
            module->Tick(Timestep(m_Duration.count()));
        }
    }

    void Application::Close()
    {
        m_Running = false;
    }
}
