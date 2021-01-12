#pragma once
#include "Interface/IRuntimeModule.h"

namespace Rocket
{
    Interface IApplication : implements IRuntimeModule
    {
    public:
        IApplication(const std::string &name = "IApplication") : IRuntimeModule(name) {}
        virtual ~IApplication() = default;

        virtual void LoadConfig() = 0;

        virtual void PreInitialize() = 0;
        virtual void PostInitialize() = 0;

        virtual void PreInitializeModule() = 0;
        virtual int InitializeModule() = 0;
        virtual void PostInitializeModule() = 0;
        virtual void FinalizeModule() = 0;

        virtual void TickModule() = 0;
        virtual void Tick() = 0;
        
        bool IsRunning() { return m_IsRunning; }
        void SetRunningState(bool state) { m_IsRunning = state; }
        void Close() { SetRunningState(false); }
    protected:
        bool m_IsRunning = true;
    };

    IApplication *CreateApplicationInstance();
} // namespace Rocket