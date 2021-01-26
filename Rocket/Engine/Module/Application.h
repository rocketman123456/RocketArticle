#pragma once
#include "Interface/IApplication.h"
#include "Event/ApplicationEvent.h"

namespace Rocket
{
    Interface Application : inheritance IApplication
    {
    public:
        Application()
        {
            RK_CORE_ASSERT(!s_Instance, "Application already exists!");
            s_Instance = this;
        }
        virtual ~Application() = default;

        virtual void LoadConfig(const Ref<ConfigLoader>& config) override;
        inline const Ref<ConfigLoader>& GetConfig() const { return m_Config; }

        virtual int Initialize() final;
        virtual void Finalize() final;

        virtual int InitializeModule() final;
        virtual void FinalizeModule() final;

        void PushModule(IRuntimeModule* module);

        virtual void TickModule(Timestep ts) final;
        virtual void Tick(Timestep ts) final;

        static Application& Get() { return *s_Instance; }

        // Event Call Back
        bool OnWindowClose(EventPtr& e);
        bool OnWindowResize(EventPtr& e);

    protected:
        bool m_Running = true;
        bool m_Minimized = false;
        bool m_Parallel = true;
        // Modules
        Vec<IRuntimeModule *> m_Modules;
        // Config
        Ref<ConfigLoader> m_Config;
        std::string m_AssetPath;
    private:
        static Application* s_Instance;
    };

    // Implement This Function to Create Different Applications
    Application* CreateApplication();
    extern Application* g_Application;
}