#pragma once
#include "Interface/IRuntimeModule.h"
#include "Common/Window.h"

namespace Rocket
{
    class WindowManager : implements IRuntimeModule
    {
    public:
        WindowManager() : IRuntimeModule("WindowManager") {}
        virtual ~WindowManager() = default;

        virtual int Initialize() final;
        virtual void Finalize() final;

        virtual void Tick(Timestep ts) final;

        const Ref<Window> GetWindow() { return m_Window; }
    private:
        Ref<Window> m_Window;
    };
}