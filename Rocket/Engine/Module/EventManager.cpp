#include "Module/EventManager.h"
#include "Module/WindowManager.h"
#include "Module/Application.h"
//#include "Module/GraphicsManager.h"
//#include "Common/Mallocator.h"

#include <GLFW/glfw3.h>

using namespace Rocket;

EventManager* EventManager::instance_ = nullptr;
ElapseTimer* Rocket::g_EventTimer;

EventManager* Rocket::GetEventManager() { return new EventManager(true); }

int EventManager::Initialize()
{
    g_EventTimer = new ElapseTimer();
    g_EventTimer->Start();

    active_event_queue_ = 0;

    window_handle_ = static_cast<GLFWwindow*>(g_WindowManager->GetNativeWindow());
    window_data_.title = Application::Get().GetName();
        
    glfwSetWindowUserPointer(window_handle_, &window_data_);

    SetupListener();
    SetupCallback();

    timer_.Start();

    return 0;
}

void EventManager::SetupCallback()
{
    // Set GLFW callbacks
	glfwSetWindowSizeCallback(window_handle_, [](GLFWwindow *window, int width, int height) {
		RK_EVENT_TRACE("glfwSetWindowSizeCallback");
        WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
	});

	glfwSetWindowContentScaleCallback(window_handle_, [](GLFWwindow *window, float xscale, float yscale) {
		RK_EVENT_TRACE("glfwSetWindowContentScaleCallback");
        WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
	});

	glfwSetWindowRefreshCallback(window_handle_, [](GLFWwindow *window) {
		RK_EVENT_TRACE("glfwSetWindowRefreshCallback");
        WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

        EventVarVec var;
        var.resize(1);
        var[0].type = Variant::TYPE_STRING_ID;
        var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "window_refresh");
        EventPtr event = CreateRef<Event>(var);

        data.event_callback(event);
	});

	glfwSetFramebufferSizeCallback(window_handle_, [](GLFWwindow *window, int width, int height) {
        RK_EVENT_TRACE("glfwSetFramebufferSizeCallback");
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

        EventVarVec var;
        var.resize(4);
        var[0].type = Variant::TYPE_STRING_ID;
        var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "window_resize");
        var[1].type = Variant::TYPE_INT32;
        var[1].asInt32 = width;
        var[2].type = Variant::TYPE_INT32;
        var[2].asInt32 = height;
        var[3].type = Variant::TYPE_INT32;
        var[3].asInt32 = 0;
        EventPtr event = CreateRef<Event>(var);

		data.event_callback(event);
	});

	glfwSetWindowCloseCallback(window_handle_, [](GLFWwindow *window) {
        RK_EVENT_TRACE("glfwSetWindowCloseCallback");
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

        EventVarVec var;
        var.resize(1);
        var[0].type = Variant::TYPE_STRING_ID;
        var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "window_close");
        EventPtr event = CreateRef<Event>(var);

        data.event_callback(event);
	});

	glfwSetKeyCallback(window_handle_, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        //RK_EVENT_TRACE("glfwSetKeyCallback");
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

        EventVarVec var;
        var.resize(3);
        var[0].type = Variant::TYPE_STRING_ID;
        var[0].asStringId = 0;
        var[1].type = Variant::TYPE_INT32;
        var[1].asInt32 = scancode;
        var[2].type = Variant::TYPE_INT32;
        var[2].asInt32 = 0;
		switch (action)
		{
            case GLFW_PRESS: {
                var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "key_press");
                var[2].asInt32 = 0;
            } break;
            case GLFW_RELEASE: {
                var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "key_release");
                var[2].asInt32 = 0;
            } break;
            case GLFW_REPEAT: {
                var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "key_repeat");
                var[2].asInt32 = 1;
            } break;
		}
        EventPtr event = CreateRef<Event>(var);
        data.event_callback(event);
	});

	glfwSetCharCallback(window_handle_, [](GLFWwindow *window, uint32_t keycode) {
        //RK_EVENT_TRACE("glfwSetCharCallback");
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

        EventVarVec var;
        var.resize(2);
        var[0].type = Variant::TYPE_STRING_ID;
        var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "key_char_code");
        var[1].type = Variant::TYPE_INT32;
        var[1].asInt32 = keycode;
        EventPtr event = CreateRef<Event>(var);

        data.event_callback(event);
	});

	glfwSetMouseButtonCallback(window_handle_, [](GLFWwindow *window, int button, int action, int mods) {
        //RK_EVENT_TRACE("glfwSetMouseButtonCallback");
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

        EventVarVec var;
        var.resize(2);
        var[0].type = Variant::TYPE_STRING_ID;
        var[0].asStringId = 0;
        var[1].type = Variant::TYPE_INT32;
        var[1].asInt32 = button;
		switch (action)
		{
            case GLFW_PRESS: {
                var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "mouse_button_press");
            } break;
            case GLFW_RELEASE: {
                var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "mouse_button_release");
            } break;
		}
        EventPtr event = CreateRef<Event>(var);
        
        data.event_callback(event);
	});

	glfwSetScrollCallback(window_handle_, [](GLFWwindow *window, double xOffset, double yOffset) {
        //RK_EVENT_TRACE("glfwSetScrollCallback");
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

        EventVarVec var;
        var.resize(3);
        var[0].type = Variant::TYPE_STRING_ID;
        var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "mouse_scroll");
        var[1].type = Variant::TYPE_DOUBLE;
        var[1].asDouble = xOffset;
        var[2].type = Variant::TYPE_DOUBLE;
        var[2].asDouble = yOffset;
        EventPtr event = CreateRef<Event>(var);
		
        data.event_callback(event);
	});

	glfwSetCursorPosCallback(window_handle_, [](GLFWwindow *window, double xPos, double yPos) {
        //RK_EVENT_TRACE("glfwSetCursorPosCallback");
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

        EventVarVec var;
        var.resize(3);
        var[0].type = Variant::TYPE_STRING_ID;
        var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "mouse_move");
        var[1].type = Variant::TYPE_DOUBLE;
        var[1].asDouble = xPos;
        var[2].type = Variant::TYPE_DOUBLE;
        var[2].asDouble = yPos;
        EventPtr event = CreateRef<Event>(var);

		data.event_callback(event);
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
    auto event_type = event->GetEventType();
    auto id_1 = GlobalHashTable::HashString("Event"_hash, "window_close");
    auto id_2 = GlobalHashTable::HashString("Event"_hash, "window_resize");
    if(event_type == id_1 || event_type == id_2)
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

    timer_.MarkLapping();
    bool result = Update();
    if(!result)
    {
        RK_EVENT_WARN("EventManager Process Unfinish!");
    }

    PROFILE_END_CPU_SAMPLE();
}

bool EventManager::Update(uint64_t maxMillis)
{
    double currMs = timer_.GetElapsedTime();
    double maxMs = currMs + maxMillis;

    // This section added to handle events from other threads.
    EventPtr pRealtimeEvent;
    while (event_thread_queue_.try_pop(pRealtimeEvent))
    {
        QueueEvent(pRealtimeEvent);
        currMs = timer_.GetElapsedTime();
        if (currMs >= maxMs)
        {
            RK_EVENT_WARN("A realtime process is spamming the event manager! {}", *pRealtimeEvent);
        }
    }

    // swap active queues and clear the new queue after the swap
    int queueToProcess = active_event_queue_;
    active_event_queue_ = (active_event_queue_ + 1) % EVENTMANAGER_NUM_QUEUES;
    event_queue_[active_event_queue_].clear();

    //RK_EVENT_INFO("Processing Event Queue {0} - {1} events to process", queueToProcess, event_queue_[queueToProcess].size());

    // Process the queue
    while (!event_queue_[queueToProcess].empty())
    {
        // pop the front of the queue
        EventPtr pEvent = event_queue_[queueToProcess].front();
        event_queue_[queueToProcess].pop_front();
        //RK_EVENT_TRACE("\tProcessing Event {0}", pEvent->GetName());

        const EventType& eventType = pEvent->GetEventType();

        // find all the delegate functions registered for this event
        auto findIt = event_listener_.find(eventType);
        if (findIt != event_listener_.end())
        {
            const EventListenerList& eventListeners = findIt->second;
            //RK_EVENT_TRACE("\tFound {0} delegates", (unsigned long)eventListeners.size());

            // call each listener
            for (auto it = eventListeners.begin(); it != eventListeners.end(); ++it)
            {
                EventListenerDelegate listener = (*it);
                //RK_EVENT_TRACE("\tSending event {0} to delegate", pEvent->GetName());
                bool processed = listener(pEvent);
                if (processed)
                    break;
            }
        }

        // check to see if time ran out
        currMs = timer_.GetElapsedTime();
        if (currMs >= maxMs)
        {
            RK_EVENT_TRACE("Aborting event processing; time ran out");
            break;
        }
    }
        
    // If we couldn't process all of the events, push the remaining events to the new active queue.
    // Note: To preserve sequencing, go back-to-front, inserting them at the head of the active queue
    bool queueFlushed = (event_queue_[queueToProcess].empty());
    if (!queueFlushed)
    {
        while (!event_queue_[queueToProcess].empty())
        {
            EventPtr pEvent = event_queue_[queueToProcess].back();
            event_queue_[queueToProcess].pop_back();
            event_queue_[active_event_queue_].push_back(pEvent);
        }
    }
        
    return queueFlushed;
}

bool EventManager::AddListener(const EventListenerDelegate& eventDelegate, const EventType& type)
{
    EventListenerList& eventListenerList = event_listener_[type];  // this will find or create the entry
    for (auto it = eventListenerList.begin(); it != eventListenerList.end(); ++it)
    {
        // Must use function pointer as listener
        if (eventDelegate == *it)
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
    auto findIt = event_listener_.find(type);
    if (findIt != event_listener_.end())
    {
        EventListenerList& listeners = findIt->second;
        for (auto it = listeners.begin(); it != listeners.end(); ++it)
        {
            // Must use function pointer as listener
            if (eventDelegate == (*it))
            {
                listeners.erase(it);
                RK_EVENT_TRACE("Successfully removed delegate function from event type {0}", GlobalHashTable::GetStringFromId("Event"_hash, type));
                success = true;
                break;
            }
        }
    }
    return success;
}

bool EventManager::TriggerEvent(EventPtr& event) const
{
    //RK_EVENT_TRACE("Trigger Event : {}", event->GetName());
    bool processed = false;
    auto findIt = event_listener_.find(event->GetEventType());
    if (findIt != event_listener_.end())
    {
        const EventListenerList& eventListenerList = findIt->second;
        for (EventListenerList::const_iterator it = eventListenerList.begin(); it != eventListenerList.end(); ++it)
        {
            auto listener = (*it);
            //RK_EVENT_TRACE("Trigger Event {0} to delegate.", event->GetName());
            processed = listener(event);  // call the delegate
            if (processed)
                break;
        }
    }
    return processed;
}

bool EventManager::QueueEvent(const EventPtr& event)
{
    RK_CORE_ASSERT(active_event_queue_ >= 0, "EventManager Active Queue Error");
    RK_CORE_ASSERT(active_event_queue_ < EVENTMANAGER_NUM_QUEUES, "EventManager Active Queue Error");

    //RK_EVENT_TRACE("Attempting to queue event: {0}", event->GetName());

    auto findIt = event_listener_.find(event->GetEventType());
    if (findIt != event_listener_.end())
    {
        event_queue_[active_event_queue_].push_back(event);
        //RK_EVENT_TRACE("Successfully queued event: {0}", event->GetName());
        return true;
    }
    else
    {
        //RK_EVENT_TRACE("Skipping event since there are no delegates registered: {0}", event->GetName());
        return false;
    }
}

bool EventManager::ThreadSafeQueueEvent(const EventPtr& event)
{
    event_thread_queue_.push(event);
    return true;
}

bool EventManager::AbortEvent(const EventType& type, bool allOfType)
{
    RK_CORE_ASSERT(active_event_queue_ >= 0, "EventManager Active Queue Error");
    RK_CORE_ASSERT(active_event_queue_ < EVENTMANAGER_NUM_QUEUES, "EventManager Active Queue Error");

    bool success = false;
    EventListenerMap::iterator findIt = event_listener_.find(type);

    if (findIt != event_listener_.end())
    {
        EventQueue& eventQueue = event_queue_[active_event_queue_];
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
