#pragma once
#include "Interface/IApplication.h"

namespace Rocket
{
    class Application : implements IApplication
    {
    public:
        Application(const std::string &name = "Application") : IApplication(name) {}
        virtual ~Application() = default;

        virtual void PreInitialize() override {}
        virtual int Initialize() override;
        virtual void PostInitialize() override {}
        virtual void Finalize() override;

        virtual void PreInitializeModule() override {}
        virtual int InitializeModule() override;
        virtual void PostInitializeModule() override {}
        virtual void FinalizeModule() override;

        void PushModule(IRuntimeModule * module);

        virtual void TickModule() override;
        virtual void Tick() override;

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
    private:
        static Application *s_Instance;
    };

    // Implement This Function to Create Different Applications
    Application* CreateApplication();
}