#pragma once
#include "Interface/IRuntimeModule.h"

#include "Event/Event.h"
#include "Utils/Timer.h"

#include <entt/entt.hpp>
#include <queue>
#include <list>
#include <map>

struct GLFWwindow;
#define EVENTMANAGER_NUM_QUEUES 2

namespace Rocket
{
    using EventCallbackFn = std::function<void(EventPtr &)>;
    using EventListenerFnptr = bool(*)(EventPtr&);
    using EventListenerDelegate = entt::delegate<bool(EventPtr&)>;
    //using EventListenerDelegate = std::function<bool(EventPtr &)>;
    using EventListenerList = std::list<EventListenerDelegate>;
    using EventListenerMap = std::map<EventType, EventListenerList>;
    using EventQueue = std::list<EventPtr>;
    
    class EventManager : implements IRuntimeModule
    {
    public:
        EventManager(bool global = true) : IRuntimeModule("EventManager"), m_Global(global) 
        {
            if(m_Global)
            {
                RK_CORE_ASSERT(!s_Instance, "Global EventManager already exists!");
                s_Instance = this;
            }
        }
        virtual ~EventManager() = default;

        virtual int Initialize() final;
        virtual void Finalize() final;

        virtual void Tick(Timestep ts) final;
        // Allow for processing of any queued messages, optionally specify a processing time limit so that the event 
        // processing does not take too long. Note the danger of using this artificial limiter is that all messages 
        // may not in fact get processed.
        //
        // returns true if all messages ready for processing were completed, false otherwise (e.g. timeout )
        bool Update(uint64_t maxMillis = 1000);

        void OnEvent(EventPtr& event);

        // Registers a delegate function that will get called when the event type is triggered.  Returns true if 
        // successful, false if not.
        bool AddListener(const EventListenerDelegate& eventDelegate, const EventType& type);

        // Removes a delegate / event type pairing from the internal tables.  Returns false if the pairing was not found.
        bool RemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type);

        // Fire off event NOW.  This bypasses the queue entirely and immediately calls all delegate functions registered 
        // for the event.
        bool TriggerEvent(EventPtr& event) const;

        // Fire off event.  This uses the queue and will call the delegate function on the next call to VTick(), assuming
        // there's enough time.
        bool QueueEvent(const EventPtr& event);
        bool ThreadSafeQueueEvent(const EventPtr& event);

        // Find the next-available instance of the named event type and remove it from the processing queue.  This 
        // may be done up to the point that it is actively being processed ...  e.g.: is safe to happen during event
        // processing itself.
        //
        // if allOfType is true, then all events of that type are cleared from the input queue.
        //
        // returns true if the event was found and removed, false otherwise
        bool AbortEvent(const EventType& type, bool allOfType = false);

        // Getter for the main global event manager.  This is the event manager that is used by the majority of the 
        // engine, though you are free to define your own as long as you instantiate it with setAsGlobal set to false.
        // It is not valid to have more than one global event manager.
        static EventManager* Get(void);

    private:
        void SetupCallback();
        void SetupListener();
        // Window attributes
		void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

    private:
        struct WindowData
        {
            std::string Title;
            EventCallbackFn EventCallback;
        };

        bool        m_Global;
        WindowData  m_Data;
        GLFWwindow* m_WindowHandle = nullptr;
        int32_t     m_ActiveEventQueue;
        EventQueue  m_EventQueue[2];
        // TODO : use thread safe queue
        EventQueue  m_EventThreadQueue;
        EventListenerMap m_EventListener;
        ProfilerTimer    m_Timer;
    private:
        static EventManager* s_Instance;
    };

    EventManager* GetEventManager();
    extern EventManager* g_EventManager;
}