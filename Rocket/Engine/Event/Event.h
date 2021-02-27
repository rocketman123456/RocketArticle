#pragma once
#include "Interface/IEvent.h"

namespace Rocket
{
    class Event : implements IEvent
    {
    public:
        Event(const EventVarPtr& var, uint32_t count) : IEvent(var, count) {}
        virtual ~Event() = default;
    };
}