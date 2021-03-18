#pragma once
#include "Module/Application.h"
#include "Utils/ThreadPool.h"

#include "RobotUI.h"
#include "RobotSerialPort.h"

namespace Rocket
{
    class SimpleApp : implements Application
    {
    public:
        RUNTIME_MODULE_TYPE(SimpleApp);
        SimpleApp() : m_ThreadPool(2) {}
        virtual ~SimpleApp() = default;

        void PreInitializeModule() final;
        void PostInitializeModule() final;

        void PreInitialize() final;
        void PostInitialize() final;

    private:
        AdvanceThreadPool m_ThreadPool;
    };
}