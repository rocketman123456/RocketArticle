#pragma once
#include "Interface/IEvent.h"
#include "Hashing.h"

namespace Rocket
{
    class Event : implements _IEvent_
    {
    public:
        Event(const EventVarPtr& var, uint32_t count) : _IEvent_(var, count) {}
        virtual ~Event() = default;

        [[nodiscard]] const std::string& GetName() const final
        { 
            return EventHashTable::GetIdString(GetEventType());
        }
    };
}