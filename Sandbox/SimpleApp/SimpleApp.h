#pragma once
#include "Module/Application.h"
#include "RobotControl.h"
#include "RobotUI.h"
namespace Rocket
{
    class SimpleApp : implements Application
    {
    public:
        RUNTIME_MODULE_TYPE(SimpleApp);
        SimpleApp() = default;
        virtual ~SimpleApp() = default;

        void PreInitializeModule() final;
        void PostInitializeModule() final;

        void PreInitialize() final;
        void PostInitialize() final;
    };
}