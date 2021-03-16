#pragma once
#include "Core/Core.h"
#include "Utils/Variant.h"

#include <functional>

namespace Rocket
{
    ENUM(EDGE_DIRECTION) { IDLE = 0, SELF, FORWARD, BACKWARD };

    struct StateEdge;
    struct StateNode;

    using TransferFunction = std::function<uint64_t(const Vec<Variant>&, const uint64_t)>; // Action, State
    using ActionFunction = std::function<bool(const Vec<Variant>&, const Vec<Variant>&)>; // Input data, Target data

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
        Vec<Variant> data;
    };

    class StateMachine
    {
    public:
        StateMachine(const String& _name) : name(_name) {}
        ~StateMachine() = default;

        void SetInitState(Ref<StateNode> init);
        void ResetToInitState();
        bool UpdateAction(const Vec<Variant>& data);
        bool UpdateEdge(const Vec<Variant>& data);
        bool GetTransferFinish() { return isTransferFinish; }

    private:
        String name;
        Ref<StateNode> initStateNode = nullptr;
        Ref<StateNode> currStateNode = nullptr;
        Ref<StateEdge> currentEdge = nullptr;
        bool isTransferFinish = true;

        // use current action and state to decide next state
        uint64_t currentAction;
        uint64_t currentState;
        uint64_t previousState;
        uint64_t nextState;
    };

#define CREATE_NODE(name,f) Ref<StateNode> name = CreateRef<StateNode>(#name); name->transferFun = f;

}