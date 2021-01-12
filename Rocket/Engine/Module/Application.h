#pragma once
#include "Interface/IApplication.h"

#include <yaml-cpp/yaml.h>

namespace Rocket
{
    class Application : implements IApplication
    {
    public:
        Application(const std::string &name = "Application") : IApplication(name) {}
        virtual ~Application() = default;

        virtual void LoadConfig() override;

        virtual int Initialize() final;
        virtual void Finalize() final;

        virtual int InitializeModule() final;
        virtual void FinalizeModule() final;

        void PushModule(IRuntimeModule * module);

        virtual void TickModule() final;
        virtual void Tick() final;
        virtual void OnEvent(Event & event) override;

        static Application &Get() { return *s_Instance; }
    protected:
        bool m_Running = true;
        bool m_Minimized = false;
        bool m_Parallel = true;

        std::chrono::steady_clock m_Clock;
        std::chrono::duration<double> m_Duration;
        std::chrono::time_point<std::chrono::steady_clock> m_CurrentTime;
        std::chrono::time_point<std::chrono::steady_clock> m_LastTime;

        std::vector<IRuntimeModule *> m_Modules;

        YAML::Node m_Config;
        std::string m_AssetPath;
    private:
        static Application *s_Instance;
    };

    // Implement This Function to Create Different Applications
    Application* CreateApplication();
}