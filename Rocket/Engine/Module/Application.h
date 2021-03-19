#pragma once
#include "Interface/IApplication.h"
#include "Interface/IEvent.h"

namespace Rocket
{
    Interface Application : inheritance IApplication
    {
    public:
        Application()
        {
            RK_CORE_ASSERT(!instance_, "Application already exists!");
            instance_ = this;
        }
        virtual ~Application() = default;

        virtual void LoadConfig(const Ref<ConfigLoader>& config) override;
        inline const Ref<ConfigLoader>& GetConfig() const { return config_; }

        virtual int Initialize() final;
        virtual void Finalize() final;

        virtual int InitializeModule() final;
        virtual void FinalizeModule() final;

        void PushModule(IRuntimeModule* module);

        virtual void Tick(Timestep ts) final;

        static Application& Get() { return *instance_; }

        // Event Call Back
        bool OnWindowClose(EventPtr& e);
        bool OnWindowResize(EventPtr& e);

    protected:
        bool running_ = true;
        bool minimized_ = false;
        bool parallel_ = true;
        // Modules
        Vec<IRuntimeModule*> modules_;
        // Config
        Ref<ConfigLoader> config_;
        String asset_path_;
        
    private:
        static Application* instance_;
    };

    // Implement This Function to Create Different Applications
    Application* CreateApplication();
    extern Application* g_Application;
}