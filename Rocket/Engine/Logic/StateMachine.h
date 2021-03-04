#pragma once
#include "Core/Core.h"
#include "Utils/Variant.h"

#include <functional>

namespace Rocket
{
    ENUM(EDGE_DIRECTION) { IDLE = 0, SELF, FORWARD, BACKWARD };

    struct StateEdge;
    struct StateNode;

    using CheckArrive = std::function<void(Ref<StateEdge>)>;
    using TransferFunction = std::function<Ref<StateNode>(const uint64_t, const uint64_t)>;
    using ActionFunction = std::function<void(const Vec<Variant>&)>;

    struct StateNode
    {
        UMap<uint64_t, Ref<StateEdge>> edgeList;
        TransferFunction transferFun;
        //CheckArrive arriveFun;
        uint64_t id;
        String name;
        bool onArrive = true;
    };

    struct StateEdge
    {
        Ref<StateNode> parent;
        Ref<StateNode> child;
        ActionFunction actionFun;
        uint64_t id;
        String name;
    };

    class StateMechine
    {
    public:
        StateMechine() = default;
        ~StateMechine() = default;

    public:
        Ref<StateNode> initStateNode;
        Ref<StateNode> currStateNode;
        bool isInTransfer = false;

        uint64_t currentState;
        uint64_t previousState;
        uint64_t nextState;
    };
}