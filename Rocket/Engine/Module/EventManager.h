#pragma once
#include "Interface/IRuntimeModule.h"

#include "Event/Event.h"
#include "Event/ApplicationEvent.h"

struct GLFWwindow;

namespace Rocket
{
    class EventManager : implements IRuntimeModule
    {
    public:
        using EventCallbackFn = std::function<void(Event &)>;

        EventManager() : IRuntimeModule("EventManager") {}
        virtual ~EventManager() = default;

        virtual int Initialize() final;
        virtual void Finalize() final;

        virtual void Tick(Timestep ts) final;

        void OnEvent(Event& event);

    private:
        void SetupCallback();
        // Window attributes
		void SetEventCallback(const EventCallbackFn &callback) { m_Data.EventCallback = callback; }

    private:
        struct WindowData
        {
            std::string Title;
            uint32_t Width = 0, Height = 0;
            float xScale = 1.0f, yScale = 1.0f;
            bool VSync = true;
            EventCallbackFn EventCallback;
        };

        WindowData  m_Data;
        GLFWwindow* m_WindowHandle = nullptr;
    };

    EventManager* GetEventManager();
    extern EventManager* g_EventManager;
}