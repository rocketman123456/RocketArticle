#pragma once
#include "Core/Core.h"
#include "Utils/Timer.h"
#include "Utils/Variant.h"

#include <utility>
#include <optional>
#include <functional>
#include <sstream>
#include <memory>
#include <vector>

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

	enum EventCategory : uint32_t
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
		
		inline bool IsInCategory(EventCategory category) { return GetCategoryFlags() & static_cast<int>(category); }

		bool Handled = false;
		double m_TimeStamp = 0.0f;
	};

	inline std::ostream &operator << (std::ostream &os, const IEvent &e)
	{
		return os << e.ToString();
	}

	// TODO : use Variant to transmit data
	using EventVar = Vec<Variant>;
	using EventVarPtr = Ref<Variant[]>;

	Interface IEvent_
	{
	public:
		IEvent(const EventVar& var) : m_Var(var) { m_TimeStamp = g_GlobalTimer->GetExactTime(); }
		virtual ~IEvent() = default;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }

		bool Handled = false;
		double m_TimeStamp = 0.0f;
		EventVar m_Var;
	};

	inline std::ostream &operator << (std::ostream &os, const IEvent_ &e)
	{
		os << e.ToString();
		for(auto var : e.m_Var)
		return os << e.ToString();
	}

	using EventPtr = Ref<IEvent>;

	template<typename T, typename... Args>
	EventPtr CreateEvent(Args&&... args) 
	{
		// TODO : use pool allocator
		return CreateScope<T>(std::forward<Args>(args)...);
	}

#define EVENT_CLASS_TYPE(type) \
	static EventType GetStaticType() { return EventType::type; }                \
	virtual EventType GetEventType() const override { return GetStaticType(); } \
	virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) \
	virtual int GetCategoryFlags() const override { return static_cast<int>(category); }

#define RK_BIND_EVENT_FN(fn) [](auto &&... args) -> decltype(auto) \
	{ return fn(std::forward<decltype(args)>(args)...); }
#define RK_BIND_EVENT_FN_CLASS(fn) [this](auto &&... args) -> decltype(auto) \
	{ return this->fn(std::forward<decltype(args)>(args)...); }
#define RK_BIND_EVENT_FN_CLASS_PTR(pointer, fn) [pointer](auto &&... args) -> decltype(auto) \
	{ return pointer->fn(std::forward<decltype(args)>(args)...); }
} // namespace Rocket
