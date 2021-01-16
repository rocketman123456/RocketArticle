#pragma once
#include "Interface/IRuntimeModule.h"

#include "Event/Event.h"

#include <entt/entt.hpp>

struct GLFWwindow;

namespace Rocket
{
    class EventManager : implements IRuntimeModule
    {
    public:
        using EventCallbackFn = std::function<void(IEvent &)>;
        using EventListenerDelegate = std::function<bool(IEvent &)>;

        EventManager(bool global = true) : IRuntimeModule("EventManager"), m_Global(global) {}
        virtual ~EventManager() = default;

        virtual int Initialize() final;
        virtual void Finalize() final;

        virtual void Tick(Timestep ts) final;

        void OnEvent(IEvent& event);

        // Registers a delegate function that will get called when the event type is triggered.  Returns true if 
        // successful, false if not.
        bool AddListener(const EventListenerDelegate eventDelegate, const EventType& type);

        // Removes a delegate / event type pairing from the internal tables.  Returns false if the pairing was not found.
        bool RemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type);

        // Fire off event NOW.  This bypasses the queue entirely and immediately calls all delegate functions registered 
        // for the event.
        bool TriggerEvent(const IEvent& pEvent) const;

        // Fire off event.  This uses the queue and will call the delegate function on the next call to VTick(), assuming
        // there's enough time.
        bool QueueEvent(const IEvent& pEvent);
        bool ThreadSafeQueueEvent(const IEvent& pEvent);

        // Find the next-available instance of the named event type and remove it from the processing queue.  This 
        // may be done up to the point that it is actively being processed ...  e.g.: is safe to happen during event
        // processing itself.
        //
        // if allOfType is true, then all events of that type are cleared from the input queue.
        //
        // returns true if the event was found and removed, false otherwise
        bool AbortEvent(const EventType& type, bool allOfType = false);

        // Allow for processing of any queued messages, optionally specify a processing time limit so that the event 
        // processing does not take too long. Note the danger of using this artificial limiter is that all messages 
        // may not in fact get processed.
        //
        // returns true if all messages ready for processing were completed, false otherwise (e.g. timeout )
        bool Update(Timestep ts);

        // Getter for the main global event manager.  This is the event manager that is used by the majority of the 
        // engine, though you are free to define your own as long as you instantiate it with setAsGlobal set to false.
        // It is not valid to have more than one global event manager.
        static EventManager* Get(void);

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

        bool        m_Global;
        WindowData  m_Data;
        GLFWwindow* m_WindowHandle = nullptr;
    private:
        static EventManager* s_Instance;
    };

    EventManager* GetEventManager();
    extern EventManager* g_EventManager;
}