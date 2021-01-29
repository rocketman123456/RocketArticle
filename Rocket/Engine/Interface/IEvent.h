#pragma once
#include "Core/Core.h"
#include "Utils/Timer.h"
#include "Utils/Variant.h"
#include "Utils/Hashing.h"

#include <utility>
#include <optional>
#include <functional>
#include <sstream>
#include <memory>
#include <vector>

namespace Rocket
{
	using EventVarPtr = Variant*;
	using EventType = uint64_t;

	Interface _IEvent_
	{
	public:
		_IEvent_(const EventVarPtr& var, uint32_t count) : Var(var), Count(count)
		{
			RK_CORE_ASSERT(count > 0, "Event Var Count Error");
			TimeStamp = g_GlobalTimer->GetExactTime();
		}
		virtual ~_IEvent_() 
		{
			delete[] Var;
		}

		[[nodiscard]] virtual EventType GetEventType() const { return Var[0].m_asStringId; }
		[[nodiscard]] virtual const std::string& GetName() const { return EventHashTable::GetStringFromId(GetEventType()); }
		[[nodiscard]] virtual const std::string& ToString() const { return GetName(); }

		[[nodiscard]] int32_t GetInt32(int index) { RK_CORE_ASSERT(index < Count, "event index error"); return Var[index].m_asInt32; }
		[[nodiscard]] uint32_t GetUInt32(int index) { RK_CORE_ASSERT(index < Count, "event index error"); return Var[index].m_asUInt32; }
		[[nodiscard]] float GetFloat(int index) { RK_CORE_ASSERT(index < Count, "event index error"); return Var[index].m_asFloat; }
		[[nodiscard]] double GetDouble(int index) { RK_CORE_ASSERT(index < Count, "event index error"); return Var[index].m_asDouble; }
		[[nodiscard]] bool GetBool(int index) { RK_CORE_ASSERT(index < Count, "event index error"); return Var[index].m_asBool; }
		[[nodiscard]] void* GetPointer(int index) { RK_CORE_ASSERT(index < Count, "event index error"); return Var[index].m_asPointer; }
		[[nodiscard]] string_id GetStringId(int index) { RK_CORE_ASSERT(index < Count, "event index error"); return Var[index].m_asStringId; }

		bool Handled = false;
		double TimeStamp = 0.0f;
		EventVarPtr Var;
		uint32_t Count;
	};

	inline std::ostream& operator<<(std::ostream& os, const _IEvent_ &e)
	{
		os << e.ToString();
		for (uint32_t i = 0; i < e.Count; ++i)
		{
			switch (e.Var[i].type)
			{
			case Variant::TYPE_INT32:
				os << "[" << e.Var[i].m_asInt32 << "]";
				break;
			case Variant::TYPE_UINT32:
				os << "[" << e.Var[i].m_asUInt32 << "]";
				break;
			case Variant::TYPE_FLOAT:
				os << "[" << e.Var[i].m_asFloat << "]";
				break;
			case Variant::TYPE_DOUBLE:
				os << "[" << e.Var[i].m_asDouble << "]";
				break;
			case Variant::TYPE_POINTER:
				os << "[" << e.Var[i].m_asPointer << "]";
				break;
			case Variant::TYPE_STRING_ID:
				os << "[" << e.Var[i].m_asStringId << "]";
				break;
			default:
				RK_EVENT_ERROR("Unknow Event Data Type");
				break;
			}
		}
		return os;
	}

	using EventPtr = Ref<_IEvent_>;

#define RK_BIND_EVENT_FN(fn) [](auto &&...args) -> decltype(auto) { return fn(std::forward<decltype(args)>(args)...); }
#define RK_BIND_EVENT_FN_CLASS(fn) [this](auto &&...args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
#define RK_BIND_EVENT_FN_CLASS_PTR(pointer, fn) [pointer](auto &&...args) -> decltype(auto) { return pointer->fn(std::forward<decltype(args)>(args)...); }
} // namespace Rocket
