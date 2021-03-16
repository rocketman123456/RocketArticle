#pragma once
#include "Interface/IRuntimeModule.h"
#include "Interface/IEvent.h"
#include "Event/Event.h"
#include "Utils/Timer.h"
#include "Utils/ThreadSafeQueue.h"
#include "Utils/Delegate.h"

#include <entt/entt.hpp>
#include <functional>
#include <queue>
#include <list>
#include <map>

struct GLFWwindow;
constexpr uint16_t EVENTMANAGER_NUM_QUEUES = 2;

namespace Rocket
{
    using EventCallbackFn = std::function<void(EventPtr &)>;
    using EventListenerFnptr = bool(*)(EventPtr&);
    using EventListenerDelegate = Delegate<bool(EventPtr&)>;
    using EventListenerList = std::list<EventListenerDelegate>;
    using EventListenerMap = Map<EventType, EventListenerList>;
    using EventQueue = std::list<EventPtr>;
    using EventThreadQueue = ThreadSafeQueue<EventPtr>;

#define REGISTER_DELEGATE_CLASS(c,f,x,name) {\
    bool ret = false;\
    EventListenerDelegate delegate; \
    delegate.Bind<c,&f>(x); \
    ret = g_EventManager->AddListener(delegate, GlobalHashTable::HashString("Event"_hash, #name)); \
    RK_CORE_ASSERT(ret, "Register Delegate "#f" To "#name" Failed");}
#define REGISTER_DELEGATE_FN(f,name) {\
    bool ret = false;\
    EventListenerDelegate delegate; \
    delegate.Bind<&f>(); \
    ret = g_EventManager->AddListener(delegate, GlobalHashTable::HashString("Event"_hash, #name)); \
    RK_CORE_ASSERT(ret, "Register Delegate "#f" To "#name" Failed");}

    class EventManager : implements IRuntimeModule
    {
    public:
        RUNTIME_MODULE_TYPE(EventManager);

        EventManager(bool global = true) : m_Global(global) 
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

        void OnEvent(EventPtr& event);

        [[maybe_unused]] bool Update(uint64_t maxMillis = 100);
        [[maybe_unused]] bool AddListener(const EventListenerDelegate& eventDelegate, const EventType& type);
        [[maybe_unused]] bool RemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type);
        [[maybe_unused]] bool TriggerEvent(EventPtr& event) const;
        [[maybe_unused]] bool QueueEvent(const EventPtr& event);
        [[maybe_unused]] bool ThreadSafeQueueEvent(const EventPtr& event);
        [[maybe_unused]] bool AbortEvent(const EventType& type, bool allOfType = false);

        // Getter for the main global event manager.
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
        GLFWwindow* m_WindowHandle = nullptr;
        WindowData  m_Data;

        int32_t     m_ActiveEventQueue;
        EventQueue  m_EventQueue[2];
        EventThreadQueue m_EventThreadQueue;
        EventListenerMap m_EventListener;
        
        ElapseTimer    m_Timer;
        
    private:
        static EventManager* s_Instance;
    };

    EventManager* GetEventManager();
    extern EventManager* g_EventManager;
}