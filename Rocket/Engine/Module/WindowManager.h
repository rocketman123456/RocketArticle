#pragma once
#include "Interface/IRuntimeModule.h"
#include "Common/Window.h"

namespace Rocket
{
    class WindowManager : implements IRuntimeModule
    {
    public:
        RUNTIME_MODULE_TYPE(WindowManager);
        WindowManager() = default;
        virtual ~WindowManager() = default;

        virtual int Initialize() final;
        virtual void Finalize() final;

        virtual void Tick(Timestep ts) final {}

        const Ref<Window> GetWindow() { return m_Window; }
        void* GetNativeWindow() { return m_Window->GetNativeWindow(); }
    private:
        Ref<Window> m_Window;
    };

    WindowManager* GetWindowManager();
    extern WindowManager* g_WindowManager;
}