# Rocket (四) 简单事件系统<br>
接上一节，为了使窗口能够顺利关闭，我们在本节添加事件系统，并将其与GLFW的回调函数连接起来，使其能够顺利运行。<br>
## 1 事件系统<br>
事件系统的调用分为两种类型，及时调用与队列调用，为了使用户尽可能快的感受到交互，输入处理类型的事件将会是立即调用，游戏事件将设计成事件队列的调用。具体代码如下：<br>
`Event.h`
```
#pragma once
#include "Core/Core.h"

#include <sstream>
#include <memory>

namespace Rocket
{
	enum class EventType : uint32_t
	{
		None = 0,
		WindowClose,
		WindowResize,
		WindowFocus,
		WindowLostFocus,
		WindowMoved,
		AppTick,
		AppUpdate,
		AppRender,
		KeyPressed,
		KeyReleased,
		KeyTyped,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled,
		AudioEvent,
	};

	static const char* EventTypeName[] = 
	{
		"None",
		"WindowClose",
		"WindowResize",
		"WindowFocus",
		"WindowLostFocus",
		"WindowMoved",
		"AppTick",
		"AppUpdate",
		"AppRender",
		"KeyPressed",
		"KeyReleased",
		"KeyTyped",
		"MouseButtonPressed",
		"MouseButtonReleased",
		"MouseMoved",
		"MouseScrolled",
		"AudioEvent",
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4),
		EventCategoryAudio = BIT(5),
	};

#define EVENT_CLASS_TYPE(type) \
	static EventType GetStaticType() { return EventType::type; }                \
	virtual EventType GetEventType() const override { return GetStaticType(); } \
	virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) \
	virtual int GetCategoryFlags() const override { return static_cast<int>(category); }

	Interface IEvent
	{
	public:
		virtual ~IEvent() = default;

		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & static_cast<int>(category);
		}

	protected:
		float m_Time;
	};

	using EventPtr = std::shared_ptr<IEvent>;

	inline std::ostream &operator<<(std::ostream &os, const IEvent &e)
	{
		return os << e.ToString();
	}
} // namespace Rocket

```
`EventManager.h`
```
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
```
`EventManager.cpp`
```
#include "Module/EventManager.h"
#include "Module/WindowManager.h"
#include "Module/Application.h"

#include "Event/ApplicationEvent.h"
#include "Event/AudioEvent.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"

#include <GLFW/glfw3.h>

namespace Rocket
{
    EventManager* EventManager::s_Instance = nullptr;

    EventManager* GetEventManager()
    {
        return new EventManager(true);
    }

    int EventManager::Initialize()
    {
        m_ActiveEventQueue = 0;

        m_WindowHandle = static_cast<GLFWwindow*>(g_WindowManager->GetNativeWindow());
        m_Data.Title = Application::Get().GetName();
        glfwMakeContextCurrent(m_WindowHandle);
        glfwSetWindowUserPointer(m_WindowHandle, &m_Data);

        SetupCallback();
        SetupListener();

        m_Timer.InitTime();

        return 0;
    }

    void EventManager::SetupCallback()
    {
        // TODO : use factory to generate events
        // Set GLFW callbacks
		glfwSetWindowSizeCallback(m_WindowHandle, [](GLFWwindow *window, int width, int height) {
			RK_CORE_TRACE("glfwSetWindowSizeCallback");
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
		});

		glfwSetWindowContentScaleCallback(m_WindowHandle, [](GLFWwindow *window, float xscale, float yscale) {
			RK_CORE_TRACE("glfwSetWindowContentScaleCallback");
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
		});

		glfwSetWindowRefreshCallback(m_WindowHandle, [](GLFWwindow *window) {
			RK_CORE_TRACE("glfwSetWindowRefreshCallback");
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
		});

		glfwSetFramebufferSizeCallback(m_WindowHandle, [](GLFWwindow *window, int width, int height) {
            RK_CORE_TRACE("glfwSetFramebufferSizeCallback");
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            EventPtr event = std::make_shared<WindowResizeEvent>(width, height, 1.0f, 1.0f);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow *window) {
            RK_CORE_TRACE("glfwSetWindowCloseCallback");
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            EventPtr event = std::make_shared<WindowCloseEvent>();
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_WindowHandle, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            RK_CORE_TRACE("glfwSetKeyCallback");
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			switch (action)
			{
                case GLFW_PRESS: {
                    EventPtr event = std::make_shared<KeyPressedEvent>(key, 0);
                    data.EventCallback(event);
                } break;
                case GLFW_RELEASE: {
                    EventPtr event = std::make_shared<KeyReleasedEvent>(key);
                    data.EventCallback(event);
                } break;
                case GLFW_REPEAT: {
                    EventPtr event = std::make_shared<KeyPressedEvent>(key, 1);
                    data.EventCallback(event);
                } break;
			}
		});

		glfwSetCharCallback(m_WindowHandle, [](GLFWwindow *window, uint32_t keycode) {
            RK_CORE_TRACE("glfwSetCharCallback");
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            EventPtr event = std::make_shared<KeyTypedEvent>(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_WindowHandle, [](GLFWwindow *window, int button, int action, int mods) {
            RK_CORE_TRACE("glfwSetMouseButtonCallback");
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			switch (action)
			{
                case GLFW_PRESS: {
                    EventPtr event = std::make_shared<MouseButtonPressedEvent>(button);
                    data.EventCallback(event);
                } break;
                case GLFW_RELEASE: {
                    EventPtr event = std::make_shared<MouseButtonReleasedEvent>(button);
                    data.EventCallback(event);
                } break;
			}
		});

		glfwSetScrollCallback(m_WindowHandle, [](GLFWwindow *window, double xOffset, double yOffset) {
            RK_CORE_TRACE("glfwSetScrollCallback");
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            EventPtr event = std::make_shared<MouseScrolledEvent>((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_WindowHandle, [](GLFWwindow *window, double xPos, double yPos) {
            RK_CORE_TRACE("glfwSetCursorPosCallback");
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            EventPtr event = std::make_shared<MouseMovedEvent>((float)xPos, (float)yPos);
			data.EventCallback(event);
		});

        this->SetEventCallback(RK_BIND_EVENT_FN_CLASS(EventManager::OnEvent));
    }

    void EventManager::SetupListener()
    {
        EventListenerDelegate delegate{entt::connect_arg<&Application::OnWindowClose>, *g_Application};
        AddListener(delegate, EventType::WindowClose);
    }

    void EventManager::Finalize()
    {
    }

    void EventManager::OnEvent(EventPtr& event)
    {
        TriggerEvent(event);
    }

    void EventManager::Tick(Timestep ts)
    {
        m_Timer.MarkTimeThisTick();
        glfwPollEvents();
        bool result = Update();
        if(!result)
            RK_CORE_WARN("EventManager Process Unfinish!");
    }

    bool EventManager::Update(uint64_t maxMillis)
    {
        float currMs = m_Timer.GetElapsedTime();
        float maxMs = currMs + maxMillis;

        // This section added to handle events from other threads.
        EventPtr pRealtimeEvent;
        // TODO : support multi-thread event handle
        //while (m_EventThreadQueue.try_pop(pRealtimeEvent))
        //{
        //    QueueEvent(pRealtimeEvent);
        //    currMs = GetTickCount();
        //    if (maxMillis != IEventManager::kINFINITE)
        //    {
        //        if (currMs >= maxMs)
        //        {
        //            RK_CORE_ERROR("A realtime process is spamming the event manager!");
        //        }
        //    }
        //}

        // swap active queues and clear the new queue after the swap
        int queueToProcess = m_ActiveEventQueue;
        m_ActiveEventQueue = (m_ActiveEventQueue + 1) % EVENTMANAGER_NUM_QUEUES;
        m_EventQueue[m_ActiveEventQueue].clear();

        RK_CORE_INFO("Processing Event Queue {0} - {1} events to process", queueToProcess, m_EventQueue[queueToProcess].size());

        // Process the queue
        while (!m_EventQueue[queueToProcess].empty())
        {
            // pop the front of the queue
            EventPtr pEvent = m_EventQueue[queueToProcess].front();
            m_EventQueue[queueToProcess].pop_front();
            RK_CORE_INFO("\t\tProcessing Event {0}", pEvent->GetName());

            const EventType& eventType = pEvent->GetEventType();

            // find all the delegate functions registered for this event
            auto findIt = m_EventListener.find(eventType);
            if (findIt != m_EventListener.end())
            {
                const EventListenerList& eventListeners = findIt->second;
                RK_CORE_INFO("\t\tFound {0} delegates", (unsigned long)eventListeners.size());

                // call each listener
                for (auto it = eventListeners.begin(); it != eventListeners.end(); ++it)
                {
                    EventListenerDelegate listener = (*it);
                    RK_CORE_INFO("\t\tSending event {0} to delegate", pEvent->GetName());
                    listener(pEvent);
                }
            }

            // check to see if time ran out
            currMs = m_Timer.GetElapsedTime();
            if (currMs >= maxMs)
            {
                RK_CORE_INFO("Aborting event processing; time ran out");
                break;
            }
        }
        
        // If we couldn't process all of the events, push the remaining events to the new active queue.
        // Note: To preserve sequencing, go back-to-front, inserting them at the head of the active queue
        bool queueFlushed = (m_EventQueue[queueToProcess].empty());
        if (!queueFlushed)
        {
            while (!m_EventQueue[queueToProcess].empty())
            {
                EventPtr pEvent = m_EventQueue[queueToProcess].back();
                m_EventQueue[queueToProcess].pop_back();
                m_EventQueue[m_ActiveEventQueue].push_back(pEvent);
            }
        }
        
        return queueFlushed;
    }

    bool EventManager::AddListener(const EventListenerDelegate& eventDelegate, const EventType& type)
    {
        bool success = false;
        EventListenerList& eventListenerList = m_EventListener[type];  // this will find or create the entry
        for (auto it = eventListenerList.begin(); it != eventListenerList.end(); ++it)
        {
            if (eventDelegate == (*it))
            {
                RK_CORE_WARN("Attempting to double-register a delegate");
                return success;
            }
        }
        eventListenerList.push_back(eventDelegate);
        success = true;
        return success;
    }

    bool EventManager::RemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type)
    {
        bool success = false;
        auto findIt = m_EventListener.find(type);
        if (findIt != m_EventListener.end())
        {
            EventListenerList& listeners = findIt->second;
            for (auto it = listeners.begin(); it != listeners.end(); ++it)
            {
                if (eventDelegate == (*it))
                {
                    listeners.erase(it);
                    RK_CORE_INFO("Successfully removed delegate function from event type {0}", EventTypeName[static_cast<uint32_t>(type)]);
                    success = true;
                    // we don't need to continue because it should be impossible 
                    // for the same delegate function to be registered for the same event more than once
                    break;
                }
            }
        }
        return success;
    }

    bool EventManager::TriggerEvent(EventPtr& event) const
    {
        bool processed = false;
        auto findIt = m_EventListener.find(event->GetEventType());
        if (findIt != m_EventListener.end())
        {
            const EventListenerList& eventListenerList = findIt->second;
            for (EventListenerList::const_iterator it = eventListenerList.begin(); it != eventListenerList.end(); ++it)
            {
                auto listener = (*it);
                RK_CORE_INFO("Sending Event {0} to delegate.", event->GetName());
                processed = listener(event);  // call the delegate
            }
        }
        return processed;
    }

    bool EventManager::QueueEvent(const EventPtr& event)
    {
        RK_CORE_ASSERT(m_ActiveEventQueue >= 0, "EventManager Active Queue Error");
        RK_CORE_ASSERT(m_ActiveEventQueue < EVENTMANAGER_NUM_QUEUES, "EventManager Active Queue Error");

        RK_CORE_INFO("Attempting to queue event: {0}", event->GetName());

        auto findIt = m_EventListener.find(event->GetEventType());
        if (findIt != m_EventListener.end())
        {
            m_EventQueue[m_ActiveEventQueue].push_back(event);
            RK_CORE_INFO("Successfully queued event: {0}", event->GetName());
            return true;
        }
        else
        {
            RK_CORE_INFO("Skipping event since there are no delegates registered to receive it: {0}", event->GetName());
            return false;
        }
    }

    bool EventManager::ThreadSafeQueueEvent(const EventPtr& event)
    {
        m_EventThreadQueue.push_back(event);
        return true;
    }

    bool EventManager::AbortEvent(const EventType& type, bool allOfType)
    {
        RK_CORE_ASSERT(m_ActiveEventQueue >= 0, "EventManager Active Queue Error");
        RK_CORE_ASSERT(m_ActiveEventQueue < EVENTMANAGER_NUM_QUEUES, "EventManager Active Queue Error");

        bool success = false;
        EventListenerMap::iterator findIt = m_EventListener.find(type);

        if (findIt != m_EventListener.end())
        {
            EventQueue& eventQueue = m_EventQueue[m_ActiveEventQueue];
            auto it = eventQueue.begin();
            while (it != eventQueue.end())
            {
                // Removing an item from the queue will invalidate the iterator, so have it point to the next member.  All
                // work inside this loop will be done using thisIt.
                auto thisIt = it;
                ++it;

                if ((*thisIt)->GetEventType() == type)
                {
                    eventQueue.erase(thisIt);
                    success = true;
                    if (!allOfType)
                        break;
                }
            }
        }

        return success;
    }
}
```