#pragma once
#include "Module/Application.h"

namespace Rocket
{
    class SimpleApp : implements Application
    {
    public:
        SimpleApp() : Application("SimpleApp") {}

        virtual void PreInitializeModule() override;
        virtual void PostInitializeModule() override;

        virtual void PreInitialize() override;
        virtual void PostInitialize() override;
    };
}