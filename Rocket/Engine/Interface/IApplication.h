#pragma once
#include "Interface/IRuntimeModule.h"
#include "Common/ConfigLoader.h"

namespace Rocket
{
    Interface IApplication : implements IRuntimeModule
    {
    public:
        IApplication() : is_running_(true) {}
        virtual ~IApplication() = default;

        virtual void LoadConfig(const Ref<ConfigLoader>& config) = 0;

        virtual void PreInitialize() = 0;
        virtual void PostInitialize() = 0;

        virtual void PreInitializeModule() = 0;
        virtual int InitializeModule() = 0;
        virtual void PostInitializeModule() = 0;
        virtual void FinalizeModule() = 0;
        
        bool IsRunning() { return is_running_; }
        void SetRunningState(bool state) { is_running_ = state; }
    protected:
        bool is_running_;
    };

    IApplication *CreateApplicationInstance();
} // namespace Rocket