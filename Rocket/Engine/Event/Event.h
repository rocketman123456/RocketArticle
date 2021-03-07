#pragma once
#include "Interface/IEvent.h"

namespace Rocket
{
    class Event : implements IEvent
    {
    public:
        Event(const EventVarVec& var) : IEvent(var) {}
        virtual ~Event() = default;
    };
}