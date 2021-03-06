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
    using TransferFunction = std::function<Ref<StateNode>(const uint64_t, const uint64_t)>; // Action, State
    using ActionFunction = std::function<void(const Vec<Variant>&)>;

    struct StateNode
    {
        UMap<uint64_t, Ref<StateEdge>> edgeList;
        TransferFunction transferFun;
        //CheckArrive arriveFun;
        uint64_t id;
        String name;
        Vec<Variant> data;
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

        void SetInitState(Ref<StateNode> init);
        void ResetToInitState();
        void Update(uint64_t action);

    private:
        Ref<StateNode> initStateNode = nullptr;
        Ref<StateNode> currStateNode = nullptr;
        bool isInTransfer = false;

        // use current action and state to decide next state
        uint64_t currentAction;
        uint64_t currentState;
        uint64_t previousState;
        uint64_t nextState;
    };
}