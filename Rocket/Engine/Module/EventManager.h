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

        EventManager(bool global = true) : global_(global) 
        {
            if(global_)
            {
                RK_CORE_ASSERT(!instance_, "Global EventManager already exists!");
                instance_ = this;
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
		void SetEventCallback(const EventCallbackFn& callback) { window_data_.event_callback = callback; }

    private:
        struct WindowData
        {
            std::string title;
            EventCallbackFn event_callback;
        };

        bool        global_;
        GLFWwindow* window_handle_ = nullptr;
        WindowData  window_data_;

        int32_t     active_event_queue_;
        EventQueue  event_queue_[2];
        EventThreadQueue event_thread_queue_;
        EventListenerMap event_listener_;
        
        ElapseTimer    timer_;
        
    private:
        static EventManager* instance_;
    };

    EventManager* GetEventManager();
    extern EventManager* g_EventManager;
}