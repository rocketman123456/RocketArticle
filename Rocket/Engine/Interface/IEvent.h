#pragma once
#include "Core/Core.h"
#include "Utils/Timer.h"
#include "Utils/Variant.h"
#include "Utils/Hashing.h"

#include <utility>
#include <optional>
#include <functional>
#include <sstream>

namespace Rocket
{
	using EventVarVec = Vec<Variant>;
	using EventVarPtr = Ref<Variant>;
	using EventType = uint64_t;

	extern ElapseTimer* g_EventTimer;

	Interface IEvent
	{
	public:
		IEvent(const EventVarVec& var) : variable(var) { time_stamp = g_EventTimer->GetExactTime(); }
		virtual ~IEvent() = default;

		[[nodiscard]] virtual EventType GetEventType() const { return variable[0].asStringId; }
		[[nodiscard]] virtual const String& GetName() const { return GlobalHashTable::GetStringFromId("Event"_hash, GetEventType()); }
		[[nodiscard]] virtual const String& ToString() const { return GetName(); }

		[[nodiscard]] int32_t GetInt32(uint32_t index) { RK_CORE_ASSERT(index < variable.size(), "event index error"); return variable[index].asInt32; }
		[[nodiscard]] uint32_t GetUInt32(uint32_t index) { RK_CORE_ASSERT(index < variable.size(), "event index error"); return variable[index].asUInt32; }
		[[nodiscard]] float GetFloat(uint32_t index) { RK_CORE_ASSERT(index < variable.size(), "event index error"); return variable[index].asFloat; }
		[[nodiscard]] double GetDouble(uint32_t index) { RK_CORE_ASSERT(index < variable.size(), "event index error"); return variable[index].asDouble; }
		[[nodiscard]] bool GetBool(uint32_t index) { RK_CORE_ASSERT(index < variable.size(), "event index error"); return variable[index].asBool; }
		[[nodiscard]] void* GetPointer(uint32_t index) { RK_CORE_ASSERT(index < variable.size(), "event index error"); return variable[index].asPointer; }
		[[nodiscard]] string_id GetStringId(uint32_t index) { RK_CORE_ASSERT(index < variable.size(), "event index error"); return variable[index].asStringId; }

		bool handled = false;
		double time_stamp = 0.0f;
		Vec<Variant> variable;
	};

	using EventPtr = Ref<IEvent>;

	inline std::ostream& operator<<(std::ostream& os, const IEvent &e)
	{
		os << e.ToString();
		for (uint32_t i = 0; i < e.variable.size(); ++i)
		{
			switch (e.variable[i].type)
			{
			case Variant::TYPE_INT32:
				os << "[" << e.variable[i].asInt32 << "]";
				break;
			case Variant::TYPE_UINT32:
				os << "[" << e.variable[i].asUInt32 << "]";
				break;
			case Variant::TYPE_FLOAT:
				os << "[" << e.variable[i].asFloat << "]";
				break;
			case Variant::TYPE_DOUBLE:
				os << "[" << e.variable[i].asDouble << "]";
				break;
			case Variant::TYPE_POINTER:
				os << "[" << e.variable[i].asPointer << "]";
				break;
			case Variant::TYPE_STRING_ID:
				os << "[" << e.variable[i].asStringId << "]";
				break;
			default:
				RK_EVENT_ERROR("Unknow Event Data Type");
				break;
			}
		}
		return os;
	}

#define RK_BIND_EVENT_FN(fn) [](auto &&...args) -> decltype(auto) { return fn(std::forward<decltype(args)>(args)...); }
#define RK_BIND_EVENT_FN_CLASS(fn) [this](auto &&...args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
#define RK_BIND_EVENT_FN_CLASS_PTR(pointer, fn) [pointer](auto &&...args) -> decltype(auto) { return pointer->fn(std::forward<decltype(args)>(args)...); }
} // namespace Rocket
