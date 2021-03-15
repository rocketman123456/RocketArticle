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
		IEvent(const EventVarVec& var) : Var(var) { TimeStamp = g_EventTimer->GetExactTime(); }
		virtual ~IEvent() = default;

		[[nodiscard]] virtual EventType GetEventType() const { return Var[0].asStringId; }
		[[nodiscard]] virtual const String& GetName() const { return GlobalHashTable::GetStringFromId("Event"_hash, GetEventType()); }
		[[nodiscard]] virtual const String& ToString() const { return GetName(); }

		[[nodiscard]] int32_t GetInt32(uint32_t index) { RK_CORE_ASSERT(index < Var.size(), "event index error"); return Var[index].asInt32; }
		[[nodiscard]] uint32_t GetUInt32(uint32_t index) { RK_CORE_ASSERT(index < Var.size(), "event index error"); return Var[index].asUInt32; }
		[[nodiscard]] float GetFloat(uint32_t index) { RK_CORE_ASSERT(index < Var.size(), "event index error"); return Var[index].asFloat; }
		[[nodiscard]] double GetDouble(uint32_t index) { RK_CORE_ASSERT(index < Var.size(), "event index error"); return Var[index].asDouble; }
		[[nodiscard]] bool GetBool(uint32_t index) { RK_CORE_ASSERT(index < Var.size(), "event index error"); return Var[index].asBool; }
		[[nodiscard]] void* GetPointer(uint32_t index) { RK_CORE_ASSERT(index < Var.size(), "event index error"); return Var[index].asPointer; }
		[[nodiscard]] string_id GetStringId(uint32_t index) { RK_CORE_ASSERT(index < Var.size(), "event index error"); return Var[index].asStringId; }

		bool Handled = false;
		double TimeStamp = 0.0f;
		Vec<Variant> Var;
	};

	using EventPtr = Ref<IEvent>;

	inline std::ostream& operator<<(std::ostream& os, const IEvent &e)
	{
		os << e.ToString();
		for (uint32_t i = 0; i < e.Var.size(); ++i)
		{
			switch (e.Var[i].type)
			{
			case Variant::TYPE_INT32:
				os << "[" << e.Var[i].asInt32 << "]";
				break;
			case Variant::TYPE_UINT32:
				os << "[" << e.Var[i].asUInt32 << "]";
				break;
			case Variant::TYPE_FLOAT:
				os << "[" << e.Var[i].asFloat << "]";
				break;
			case Variant::TYPE_DOUBLE:
				os << "[" << e.Var[i].asDouble << "]";
				break;
			case Variant::TYPE_POINTER:
				os << "[" << e.Var[i].asPointer << "]";
				break;
			case Variant::TYPE_STRING_ID:
				os << "[" << e.Var[i].asStringId << "]";
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
