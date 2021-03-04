#pragma once
#include "Core/Core.h"
#include "Utils/Variant.h"

#include <functional>

namespace Rocket
{
    ENUM(EDGE_DIRECTION) { SELF = 0, FORWARD, BACKWARD };

    struct State
    {
        Vec<Ref<State>> parent;
        Vec<Ref<State>> child;
        std::function<EDGE_DIRECTION(const Vec<Variant>&)> transferFun;
        std::function<void(void)> actionFun;
        uint64_t id;
    };

    class StateMechine
    {
    public:
        StateMechine() = default;
        ~StateMechine() = default;

    public:
        Ref<State> initState;
    };
}