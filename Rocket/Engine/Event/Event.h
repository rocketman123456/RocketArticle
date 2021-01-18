#pragma once
#include "Core/Core.h"
#include "Utils/Timer.h"

#include <sstream>
#include <memory>

namespace Rocket
{
	enum class EventType : uint32_t
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		AudioEvent,
	};

	static const char* EventTypeName[] = 
	{
		"None",
		"WindowClose", "WindowResize", "WindowFocus", "WindowLostFocus", "WindowMoved",
		"AppTick", "AppUpdate", "AppRender",
		"KeyPressed", "KeyReleased", "KeyTyped",
		"MouseButtonPressed", "MouseButtonReleased", "MouseMoved", "MouseScrolled",
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

	Interface IEvent
	{
	public:
		IEvent() { m_TimeStamp = g_GlobalTimer->GetExactTime(); }
		virtual ~IEvent() = default;


		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }
		
		inline double GetTimeStamp() { return m_TimeStamp; }
		inline bool IsInCategory(EventCategory category) { return GetCategoryFlags() & static_cast<int>(category); }

	public:
		bool Handled = false;
		double m_TimeStamp = 0.0f;
	};

	using EventPtr = Ref<IEvent>;

	template<typename T, typename... Args>
	EventPtr CreateEvent(Args&&... args) 
	{
		return CreateScope<T>(std::forward<Args>(args)...);
	}

#define EVENT_CLASS_TYPE(type) \
	static EventType GetStaticType() { return EventType::type; }                \
	virtual EventType GetEventType() const override { return GetStaticType(); } \
	virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) \
	virtual int GetCategoryFlags() const override { return static_cast<int>(category); }

	inline std::ostream &operator<<(std::ostream &os, const IEvent &e)
	{
		return os << e.ToString();
	}
} // namespace Rocket
