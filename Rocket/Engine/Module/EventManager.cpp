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
            //RK_CORE_TRACE("glfwSetFramebufferSizeCallback");
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            EventPtr event = CreateEvent<WindowResizeEvent>(width, height, 1.0f, 1.0f);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow *window) {
            //RK_CORE_TRACE("glfwSetWindowCloseCallback");
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            EventPtr event = CreateEvent<WindowCloseEvent>();
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_WindowHandle, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            //RK_CORE_TRACE("glfwSetKeyCallback");
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			switch (action)
			{
                case GLFW_PRESS: {
                    EventPtr event = CreateEvent<KeyPressedEvent>(key, 0);
                    data.EventCallback(event);
                } break;
                case GLFW_RELEASE: {
                    EventPtr event = CreateEvent<KeyReleasedEvent>(key);
                    data.EventCallback(event);
                } break;
                case GLFW_REPEAT: {
                    EventPtr event = CreateEvent<KeyPressedEvent>(key, 1);
                    data.EventCallback(event);
                } break;
			}
		});

		glfwSetCharCallback(m_WindowHandle, [](GLFWwindow *window, uint32_t keycode) {
            //RK_CORE_TRACE("glfwSetCharCallback");
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            EventPtr event = CreateEvent<KeyTypedEvent>(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_WindowHandle, [](GLFWwindow *window, int button, int action, int mods) {
            //RK_CORE_TRACE("glfwSetMouseButtonCallback");
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			switch (action)
			{
                case GLFW_PRESS: {
                    EventPtr event = CreateEvent<MouseButtonPressedEvent>(button);
                    data.EventCallback(event);
                } break;
                case GLFW_RELEASE: {
                    EventPtr event = CreateEvent<MouseButtonReleasedEvent>(button);
                    data.EventCallback(event);
                } break;
			}
		});

		glfwSetScrollCallback(m_WindowHandle, [](GLFWwindow *window, double xOffset, double yOffset) {
            //RK_CORE_TRACE("glfwSetScrollCallback");
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            EventPtr event = CreateEvent<MouseScrolledEvent>((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_WindowHandle, [](GLFWwindow *window, double xPos, double yPos) {
            //RK_CORE_TRACE("glfwSetCursorPosCallback");
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            EventPtr event = CreateEvent<MouseMovedEvent>((float)xPos, (float)yPos);
			data.EventCallback(event);
		});

        this->SetEventCallback(RK_BIND_EVENT_FN_CLASS(EventManager::OnEvent));
    }

    void EventManager::SetupListener()
    {
        AddListener(REGISTER_DELEGATE(Application::OnWindowClose, *g_Application), EventType::WindowClose);
    }

    void EventManager::Finalize()
    {
    }

    void EventManager::OnEvent(EventPtr& event)
    {
        RK_CORE_TRACE("Callback Event {0}", event->ToString());
        //TriggerEvent(event);
        QueueEvent(event);
    }

    void EventManager::Tick(Timestep ts)
    {
        PROFILE_BEGIN_CPU_SAMPLE(EventManagerUpdate, 0);

        glfwPollEvents();

        m_Timer.MarkTimeThisTick();
        bool result = Update();
        if(!result)
        {
            RK_CORE_WARN("EventManager Process Unfinish!");
        }

        PROFILE_END_CPU_SAMPLE();
    }

    bool EventManager::Update(uint64_t maxMillis)
    {
        float currMs = m_Timer.GetElapsedTime();
        float maxMs = currMs + maxMillis;

        // This section added to handle events from other threads.
        // TODO : support multi-thread event handle
        //EventPtr pRealtimeEvent;
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

        //RK_CORE_INFO("Processing Event Queue {0} - {1} events to process", queueToProcess, m_EventQueue[queueToProcess].size());

        // Process the queue
        while (!m_EventQueue[queueToProcess].empty())
        {
            // pop the front of the queue
            EventPtr pEvent = m_EventQueue[queueToProcess].front();
            m_EventQueue[queueToProcess].pop_front();
            RK_CORE_INFO("\tProcessing Event {0}", pEvent->GetName());

            const EventType& eventType = pEvent->GetEventType();

            // find all the delegate functions registered for this event
            auto findIt = m_EventListener.find(eventType);
            if (findIt != m_EventListener.end())
            {
                const EventListenerList& eventListeners = findIt->second;
                RK_CORE_INFO("\tFound {0} delegates", (unsigned long)eventListeners.size());

                // call each listener
                for (auto it = eventListeners.begin(); it != eventListeners.end(); ++it)
                {
                    EventListenerDelegate listener = (*it);
                    RK_CORE_INFO("\tSending event {0} to delegate", pEvent->GetName());
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
        EventListenerList& eventListenerList = m_EventListener[type];  // this will find or create the entry
        for (auto it = eventListenerList.begin(); it != eventListenerList.end(); ++it)
        {
            if (eventDelegate == (*it))
            {
                RK_CORE_WARN("Attempting to double-register a delegate");
                return false;
            }
        }
        eventListenerList.push_back(eventDelegate);
        return true;
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
            RK_CORE_INFO("Skipping event since there are no delegates registered: {0}", event->GetName());
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