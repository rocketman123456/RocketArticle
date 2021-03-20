#pragma once
#include "Interface/IRuntimeModule.h"
#include "Common/Window.h"
#include "Event/Event.h"

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
        virtual void Tick(Timestep ts) final;

        bool OnWindowResize(EventPtr& e);

        uint32_t GetWindowWidth() { return window_->GetWidth(); }
        uint32_t GetWindowHeight() { return window_->GetHeight(); }

        const Ref<Window> GetWindow() { return window_; }
        void* GetNativeWindow() { return window_->GetNativeWindow(); }
    private:
        Ref<Window> window_;
    };

    WindowManager* GetWindowManager();
    extern WindowManager* g_WindowManager;
}