#include "Module/EventManager.h"
#include "Module/WindowManager.h"
#include "Module/Application.h"
//#include "Module/GraphicsManager.h"
//#include "Common/Mallocator.h"

#include <GLFW/glfw3.h>

using namespace Rocket;

EventManager* EventManager::s_Instance = nullptr;
ElapseTimer* Rocket::g_EventTimer;

EventManager* Rocket::GetEventManager() { return new EventManager(true); }

int EventManager::Initialize()
{
    g_EventTimer = new Rocket::ElapseTimer();
    g_EventTimer->Start();

    m_ActiveEventQueue = 0;

    m_WindowHandle = static_cast<GLFWwindow*>(g_WindowManager->GetNativeWindow());
    m_Data.Title = Application::Get().GetName();
        
    glfwSetWindowUserPointer(m_WindowHandle, &m_Data);

    SetupCallback();
    SetupListener();

    m_Timer.Start();

    return 0;
}

void EventManager::SetupCallback()
{
    // Set GLFW callbacks
	glfwSetWindowSizeCallback(m_WindowHandle, [](GLFWwindow *window, int width, int height) {
		RK_EVENT_TRACE("glfwSetWindowSizeCallback");
        WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
	});

	glfwSetWindowContentScaleCallback(m_WindowHandle, [](GLFWwindow *window, float xscale, float yscale) {
		RK_EVENT_TRACE("glfwSetWindowContentScaleCallback");
        WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
	});

	glfwSetWindowRefreshCallback(m_WindowHandle, [](GLFWwindow *window) {
		RK_EVENT_TRACE("glfwSetWindowRefreshCallback");
        WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

        EventVarPtr ptr = Ref<Variant>(new Variant[1], [](Variant* v){ delete[]v; });
        ptr.get()[0].type = Variant::TYPE_STRING_ID;
        ptr.get()[0].m_asStringId = EventHashTable::HashString("window_refresh");
        EventPtr event = CreateRef<Event>(ptr, 1);

        data.EventCallback(event);
	});

	glfwSetFramebufferSizeCallback(m_WindowHandle, [](GLFWwindow *window, int width, int height) {
        RK_EVENT_TRACE("glfwSetFramebufferSizeCallback");
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

        EventVarPtr ptr = Ref<Variant>(new Variant[4], [](Variant* v){ delete[]v; });
        ptr.get()[0].type = Variant::TYPE_STRING_ID;
        ptr.get()[0].m_asStringId = EventHashTable::HashString("window_resize");
        ptr.get()[1].type = Variant::TYPE_INT32;
        ptr.get()[1].m_asInt32 = width;
        ptr.get()[2].type = Variant::TYPE_INT32;
        ptr.get()[2].m_asInt32 = height;
        ptr.get()[3].type = Variant::TYPE_INT32;
        ptr.get()[3].m_asInt32 = 0;
        EventPtr event = CreateRef<Event>(ptr, 4);

		data.EventCallback(event);
	});

	glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow *window) {
        RK_EVENT_TRACE("glfwSetWindowCloseCallback");
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

        EventVarPtr ptr = Ref<Variant>(new Variant[1], [](Variant* v){ delete[]v; });
        ptr.get()[0].type = Variant::TYPE_STRING_ID;
        ptr.get()[0].m_asStringId = EventHashTable::HashString("window_close");
        EventPtr event = CreateRef<Event>(ptr, 1);

        data.EventCallback(event);
	});

	glfwSetKeyCallback(m_WindowHandle, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        //RK_EVENT_TRACE("glfwSetKeyCallback");
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
        EventVarPtr ptr = Ref<Variant>(new Variant[3], [](Variant* v){ delete[]v; });
        ptr.get()[0].type = Variant::TYPE_STRING_ID;
        ptr.get()[0].m_asStringId = 0;
        ptr.get()[1].type = Variant::TYPE_INT32;
        ptr.get()[1].m_asInt32 = scancode;
        ptr.get()[2].type = Variant::TYPE_INT32;
        ptr.get()[2].m_asInt32 = 0;
		switch (action)
		{
            case GLFW_PRESS: {
                ptr.get()[0].m_asStringId = EventHashTable::HashString("key_press");
                ptr.get()[2].m_asInt32 = 0;
            } break;
            case GLFW_RELEASE: {
                ptr.get()[0].m_asStringId = EventHashTable::HashString("key_release");
                ptr.get()[2].m_asInt32 = 0;
            } break;
            case GLFW_REPEAT: {
                ptr.get()[0].m_asStringId = EventHashTable::HashString("key_repeat");
                ptr.get()[2].m_asInt32 = 1;
            } break;
		}
        EventPtr event = CreateRef<Event>(ptr, 3);
        data.EventCallback(event);
	});

	glfwSetCharCallback(m_WindowHandle, [](GLFWwindow *window, uint32_t keycode) {
        //RK_EVENT_TRACE("glfwSetCharCallback");
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
        EventVarPtr ptr = Ref<Variant>(new Variant[2], [](Variant* v){ delete[]v; });
        ptr.get()[0].type = Variant::TYPE_STRING_ID;
        ptr.get()[0].m_asStringId = EventHashTable::HashString("key_char_code");
        ptr.get()[1].type = Variant::TYPE_UINT32;
        ptr.get()[1].m_asUInt32 = keycode;
        EventPtr event = CreateRef<Event>(ptr, 2);
        data.EventCallback(event);
	});

	glfwSetMouseButtonCallback(m_WindowHandle, [](GLFWwindow *window, int button, int action, int mods) {
        //RK_EVENT_TRACE("glfwSetMouseButtonCallback");
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
        EventVarPtr ptr = Ref<Variant>(new Variant[2], [](Variant* v){ delete[]v; });
        ptr.get()[0].type = Variant::TYPE_STRING_ID;
        ptr.get()[0].m_asStringId = 0;
        ptr.get()[1].type = Variant::TYPE_INT32;
        ptr.get()[1].m_asInt32 = button;
		switch (action)
		{
            case GLFW_PRESS: {
                ptr.get()[0].m_asStringId = EventHashTable::HashString("mouse_button_press");
            } break;
            case GLFW_RELEASE: {
                ptr.get()[0].m_asStringId = EventHashTable::HashString("mouse_button_release");
            } break;
		}
        EventPtr event = CreateRef<Event>(ptr, 2);
        data.EventCallback(event);
	});

	glfwSetScrollCallback(m_WindowHandle, [](GLFWwindow *window, double xOffset, double yOffset) {
        //RK_EVENT_TRACE("glfwSetScrollCallback");
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);      
        EventVarPtr ptr = Ref<Variant>(new Variant[3], [](Variant* v){ delete[]v; });
        ptr.get()[0].type = Variant::TYPE_STRING_ID;
        ptr.get()[0].m_asStringId = EventHashTable::HashString("mouse_scroll");
        ptr.get()[1].type = Variant::TYPE_DOUBLE;
        ptr.get()[1].m_asDouble = xOffset;
        ptr.get()[2].type = Variant::TYPE_DOUBLE;
        ptr.get()[2].m_asDouble = yOffset;
        EventPtr event = CreateRef<Event>(ptr, 3);		
        data.EventCallback(event);
	});

	glfwSetCursorPosCallback(m_WindowHandle, [](GLFWwindow *window, double xPos, double yPos) {
        //RK_EVENT_TRACE("glfwSetCursorPosCallback");
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
        EventVarPtr ptr = Ref<Variant>(new Variant[3], [](Variant* v){ delete[]v; });
        ptr.get()[0].type = Variant::TYPE_STRING_ID;
        ptr.get()[0].m_asStringId = EventHashTable::HashString("mouse_move");
        ptr.get()[1].type = Variant::TYPE_DOUBLE;
        ptr.get()[1].m_asDouble = xPos;
        ptr.get()[2].type = Variant::TYPE_DOUBLE;
        ptr.get()[2].m_asDouble = yPos;
        EventPtr event = CreateRef<Event>(ptr, 3);
		data.EventCallback(event);
	});
}

void EventManager::SetupListener()
{
    SetEventCallback(std::bind(&EventManager::OnEvent, this, std::placeholders::_1));
}

void EventManager::Finalize()
{
    delete g_EventTimer;
}

void EventManager::OnEvent(EventPtr& event)
{
    bool ret = false;
    //RK_EVENT_TRACE("Callback Event {0}", *event);
    if(event->GetEventType() == EventHashTable::HashString("window_close"))
        ret = QueueEvent(event);
    else
        ret = TriggerEvent(event);
    //RK_EVENT_TRACE("On Event Callback Result : {}", ret);
}

void EventManager::Tick(Timestep ts)
{
    PROFILE_BEGIN_CPU_SAMPLE(EventManagerUpdate, 0);

    Rocket::g_EventTimer->MarkLapping();

    //glfwPollEvents();

    m_Timer.MarkLapping();
    bool result = Update();
    if(!result)
    {
        RK_EVENT_WARN("EventManager Process Unfinish!");
    }

    PROFILE_END_CPU_SAMPLE();
}

bool EventManager::Update(uint64_t maxMillis)
{
    double currMs = m_Timer.GetElapsedTime();
    double maxMs = currMs + maxMillis;

    // This section added to handle events from other threads.
    EventPtr pRealtimeEvent;
    while (m_EventThreadQueue.try_pop(pRealtimeEvent))
    {
        QueueEvent(pRealtimeEvent);
        currMs = m_Timer.GetElapsedTime();
        if (currMs >= maxMs)
        {
            RK_EVENT_WARN("A realtime process is spamming the event manager! {}", *pRealtimeEvent);
        }
    }

    // swap active queues and clear the new queue after the swap
    int queueToProcess = m_ActiveEventQueue;
    m_ActiveEventQueue = (m_ActiveEventQueue + 1) % EVENTMANAGER_NUM_QUEUES;
    m_EventQueue[m_ActiveEventQueue].clear();

    //RK_EVENT_INFO("Processing Event Queue {0} - {1} events to process", queueToProcess, m_EventQueue[queueToProcess].size());

    // Process the queue
    while (!m_EventQueue[queueToProcess].empty())
    {
        // pop the front of the queue
        EventPtr pEvent = m_EventQueue[queueToProcess].front();
        m_EventQueue[queueToProcess].pop_front();
        RK_EVENT_INFO("\tProcessing Event {0}", pEvent->GetName());

        const EventType& eventType = pEvent->GetEventType();

        // find all the delegate functions registered for this event
        auto findIt = m_EventListener.find(eventType);
        if (findIt != m_EventListener.end())
        {
            const EventListenerList& eventListeners = findIt->second;
            RK_EVENT_INFO("\tFound {0} delegates", (unsigned long)eventListeners.size());

            // call each listener
            for (auto it = eventListeners.begin(); it != eventListeners.end(); ++it)
            {
                EventListenerDelegate listener = (*it);
                RK_EVENT_INFO("\tSending event {0} to delegate", pEvent->GetName());
                bool processed = listener(pEvent);
                if (processed)
                    break;
            }
        }

        // check to see if time ran out
        currMs = m_Timer.GetElapsedTime();
        if (currMs >= maxMs)
        {
            RK_EVENT_INFO("Aborting event processing; time ran out");
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
        // Must use function pointer as listener
        if (eventDelegate == (*it))
        {
            RK_EVENT_WARN("Attempting to double-register a delegate");
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
            // Must use function pointer as listener
            if (eventDelegate == (*it))
            {
                listeners.erase(it);
                RK_EVENT_INFO("Successfully removed delegate function from event type {0}", EventHashTable::GetStringFromId(type));
                success = true;
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
            RK_EVENT_INFO("Sending Event {0} to delegate.", event->GetName());
            processed = listener(event);  // call the delegate
            if (processed)
                break;
        }
    }
    return processed;
}

bool EventManager::QueueEvent(const EventPtr& event)
{
    RK_CORE_ASSERT(m_ActiveEventQueue >= 0, "EventManager Active Queue Error");
    RK_CORE_ASSERT(m_ActiveEventQueue < EVENTMANAGER_NUM_QUEUES, "EventManager Active Queue Error");

    RK_EVENT_INFO("Attempting to queue event: {0}", event->GetName());

    auto findIt = m_EventListener.find(event->GetEventType());
    if (findIt != m_EventListener.end())
    {
        m_EventQueue[m_ActiveEventQueue].push_back(event);
        RK_EVENT_INFO("Successfully queued event: {0}", event->GetName());
        return true;
    }
    else
    {
        RK_EVENT_INFO("Skipping event since there are no delegates registered: {0}", event->GetName());
        return false;
    }
}

bool EventManager::ThreadSafeQueueEvent(const EventPtr& event)
{
    m_EventThreadQueue.push(event);
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
