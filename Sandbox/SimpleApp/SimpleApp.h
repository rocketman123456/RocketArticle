#pragma once
#include "Module/Application.h"

namespace Rocket
{
    class SimpleApp : implements Application
    {
    public:
        RUNTIME_MODULE_TYPE(SimpleApp);
        SimpleApp() = default;
        virtual ~SimpleApp() = default;

        virtual void PreInitializeModule() override;
        virtual void PostInitializeModule() override;

        virtual void PreInitialize() override;
        virtual void PostInitialize() override;
    };
}