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
	//enum class EventType : uint32_t
	//{
	//	None = 0,
	//	WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
	//	AppTick, AppUpdate, AppRender,
	//	KeyPressed, KeyReleased, KeyTyped,
	//	MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
	//	AudioEvent,
	//};
//
	//static const char* EventTypeName[] = 
	//{
	//	"None",
	//	"WindowClose", "WindowResize", "WindowFocus", "WindowLostFocus", "WindowMoved",
	//	"AppTick", "AppUpdate", "AppRender",
	//	"KeyPressed", "KeyReleased", "KeyTyped",
	//	"MouseButtonPressed", "MouseButtonReleased", "MouseMoved", "MouseScrolled",
	//	"AudioEvent",
	//};

	//enum EventCategory : uint32_t
	//{
	//	None = 0,
	//	EventCategoryApplication = RK_BIT(0),
	//	EventCategoryInput = RK_BIT(1),
	//	EventCategoryKeyboard = RK_BIT(2),
	//	EventCategoryMouse = RK_BIT(3),
	//	EventCategoryMouseButton = RK_BIT(4),
	//	EventCategoryAudio = RK_BIT(5),
	//};

	//Interface IEvent
	//{
	//public:
	//	IEvent() { TimeStamp = g_GlobalTimer->GetExactTime(); }
	//	virtual ~IEvent() = default;
//
	//	virtual EventType GetEventType() const = 0;
	//	virtual const char* GetName() const = 0;
	//	virtual int GetCategoryFlags() const = 0;
	//	virtual std::string ToString() const { return GetName(); }
	//	
	//	inline bool IsInCategory(EventCategory category) { return GetCategoryFlags() & static_cast<int>(category); }
//
	//	bool Handled = false;
	//	double TimeStamp = 0.0f;
	//};

	//inline std::ostream &operator << (std::ostream &os, const IEvent &e)
	//{
	//	return os << e.ToString();
	//}

	// TODO : use Variant to transmit data
	using EventVar = Vec<Variant>;
	using EventVarPtr = Ref<Variant[]>;

	Interface _IEvent_
	{
	public:
		_IEvent_(const EventVarPtr& var, uint32_t count) : Var(var), Count(count) 
		{
			RK_CORE_ASSERT(count > 0, "Event Var Count Error");
			TimeStamp = g_GlobalTimer->GetExactTime();
		}
		virtual ~_IEvent_() = default;

		[[nodiscard]] virtual string_id GetEventType() const 
		{
			return Var[0].m_asStringId;
		}
		
		[[nodiscard]] virtual const std::string& GetName() const = 0;
		[[nodiscard]] virtual std::string ToString() const { return GetName(); }

		bool Handled = false;
		double TimeStamp = 0.0f;
		EventVarPtr Var;
		uint32_t Count;
	};

	inline std::ostream &operator << (std::ostream &os, const _IEvent_ &e)
	{
		os << e.ToString();
		for(uint32_t i = 0; i < e.Count; ++i)
		{
			switch(e.Var[i].type)
			{
				case Variant::TYPE_INT32:
					os << "[" << e.Var[i].m_asInt32 << "]"; break;
				case Variant::TYPE_UINT32:
					os << "[" << e.Var[i].m_asUInt32 << "]"; break;
				case Variant::TYPE_FLOAT:
					os << "[" << e.Var[i].m_asFloat << "]"; break;
				case Variant::TYPE_DOUBLE:
					os << "[" << e.Var[i].m_asDouble << "]"; break;
				case Variant::TYPE_POINTER:
					os << "[" << e.Var[i].m_asPointer << "]"; break;
				case Variant::TYPE_STRING_ID:
					os << "[" << e.Var[i].m_asStringId << "]"; break;
			}
		}
		return os;
	}

	using EventPtr = Ref<_IEvent_>;
	using EventType = string_id;

	template<typename T, typename... Args>
	EventPtr CreateEvent(Args&&... args) 
	{
		// TODO : use pool allocator
		return CreateScope<T>(std::forward<Args>(args)...);
	}

//#define EVENT_CLASS_TYPE(type) \
//	static EventType GetStaticType() { return EventType::type; }                \
//	virtual EventType GetEventType() const override { return GetStaticType(); } \
//	virtual const char* GetName() const override { return #type; }
//
//#define EVENT_CLASS_CATEGORY(category) \
//	virtual int GetCategoryFlags() const override { return static_cast<int>(category); }

#define RK_BIND_EVENT_FN(fn) [](auto &&... args) -> decltype(auto) \
	{ return fn(std::forward<decltype(args)>(args)...); }
#define RK_BIND_EVENT_FN_CLASS(fn) [this](auto &&... args) -> decltype(auto) \
	{ return this->fn(std::forward<decltype(args)>(args)...); }
#define RK_BIND_EVENT_FN_CLASS_PTR(pointer, fn) [pointer](auto &&... args) -> decltype(auto) \
	{ return pointer->fn(std::forward<decltype(args)>(args)...); }
} // namespace Rocket
