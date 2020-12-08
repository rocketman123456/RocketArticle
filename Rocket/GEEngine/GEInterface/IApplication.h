#pragma once
#include "GEInterface/IRuntimeModule.h"

namespace Rocket {
    Interface IApplication : implements IRuntimeModule
    {
    public:
        virtual void PreInitializeModule() = 0;
        virtual int InitializeModule() = 0;
        virtual void PostInitializeModule() = 0;
        virtual void FinalizeModule() = 0;

        virtual void PreInitialize() = 0;
        virtual int Initialize() = 0;
        virtual void PostInitialize() = 0;
        virtual void Finalize() = 0;

        // One cycle of the main loop
        virtual void Tick() = 0;

        virtual void OnEvent(Event& event) = 0;

        virtual bool IsQuit() = 0;
    };

    IApplication* CreateApplicationInstance();
}