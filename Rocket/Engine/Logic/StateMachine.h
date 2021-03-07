#pragma once
#include "Core/Core.h"
#include "Utils/Variant.h"

#include <functional>

namespace Rocket
{
    ENUM(EDGE_DIRECTION) { IDLE = 0, SELF, FORWARD, BACKWARD };

    struct StateEdge;
    struct StateNode;

    using TransferFunction = std::function<Ref<StateEdge>(const Vec<Variant>&, const uint64_t)>; // Action, State
    using ActionFunction = std::function<bool(const Vec<Variant>&)>;

    struct StateNode
    {
        StateNode(const String& _name);
        void AddEgde(const Ref<StateEdge>& edge);
        Ref<StateEdge> GetEdge(uint64_t id);

        UMap<uint64_t, Ref<StateEdge>> edgeList;
        TransferFunction transferFun;
        uint64_t id;
        String name;
        Vec<Variant> data;
        bool onArrive = true;
    };

    struct StateEdge
    {
        StateEdge(const String& _name);

        Ref<StateNode> parent;
        Ref<StateNode> child;
        ActionFunction actionFun;
        uint64_t id;
        String name;
        bool finished = false;
    };

    class StateMachine
    {
    public:
        StateMachine(const String& _name) : name(_name) {}
        ~StateMachine() = default;

        void SetInitState(Ref<StateNode> init);
        void ResetToInitState();
        bool Update(const Vec<Variant>& data);
        bool GetIsInTransfer() { return isInTransfer; }

    private:
        String name;
        Ref<StateNode> initStateNode = nullptr;
        Ref<StateNode> currStateNode = nullptr;
        Ref<StateEdge> currentEdge = nullptr;
        bool isInTransfer = false;

        // use current action and state to decide next state
        uint64_t currentAction;
        uint64_t currentState;
        uint64_t previousState;
        uint64_t nextState;
    };
}