#pragma once
#include "Interface/IRuntimeModule.h"
#include "Common/ConfigLoader.h"

namespace Rocket
{
    Interface IApplication : implements IRuntimeModule
    {
    public:
        IApplication() : m_IsRunning(true) {}
        virtual ~IApplication() = default;

        virtual void LoadConfig(const Ref<ConfigLoader>& config) = 0;

        virtual void PreInitialize() = 0;
        virtual void PostInitialize() = 0;

        virtual void PreInitializeModule() = 0;
        virtual int InitializeModule() = 0;
        virtual void PostInitializeModule() = 0;
        virtual void FinalizeModule() = 0;

        virtual void TickModule(Timestep ts) = 0;
        
        bool IsRunning() { return m_IsRunning; }
        void SetRunningState(bool state) { m_IsRunning = state; }
    protected:
        bool m_IsRunning;
    };

    IApplication *CreateApplicationInstance();
} // namespace Rocket