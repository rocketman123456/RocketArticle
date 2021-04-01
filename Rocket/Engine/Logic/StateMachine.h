#pragma once
#include "Core/Core.h"
#include "Math/GeomMath.h"
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

        UMap<uint64_t, Ref<StateEdge>> edge_list;
        TransferFunction transfer_function;
        uint64_t id;
        String name;
        Vec<Variant> data;
    };

    struct StateEdge
    {
        StateEdge(const String& _name);

        Ref<StateNode> parent;
        Ref<StateNode> child;
        ActionFunction action_function;
        uint64_t id;
        String name;
        String action_name;
        bool finished = false;
        Vec<Variant> data;
    };

    class StateMachineSerializer;

    class StateMachine
    {
    public:
        StateMachine(const String& name) : name_(name) {}
        ~StateMachine() = default;

        void SetInitState(Ref<StateNode> init);
        void ResetToInitState();
        bool UpdateActionData(const Vec<Variant>& data);
        bool UpdateStateEdge(const Vec<Variant>& data);
        bool GetTransferFinish() { return is_transfer_finish_; }

        Ref<StateNode> GetInitNode() { return init_state_node_; }
        Ref<StateNode> GetCurrentNode() { return curr_state_node_; }
        Ref<StateEdge> GetCurrentEdge() { return current_edge_; }

        // For Node and Edge Function
        uint64_t update_along_mat(const Vec<Variant>& data, const uint64_t state);
        bool action_on_edge(const Vec<Variant>& input, const Vec<Variant>& target);

    private:
        static uint64_t Get2Mat(const UMap<uint64_t, uint64_t>& map, uint64_t input);
        static uint64_t Get2Map(const UMap<uint64_t, uint64_t>& map, uint64_t input);
        static bool is_near(float i, float j, float esp = 1e-1);

    private:
        String name_;
        Ref<StateNode> init_state_node_ = nullptr;
        Ref<StateNode> curr_state_node_ = nullptr;
        Ref<StateEdge> current_edge_ = nullptr;
        bool is_transfer_finish_ = true;

        // use current action and state to decide next state
        uint64_t current_action_;
        uint64_t current_state_;
        uint64_t previous_state_;
        uint64_t next_state_;

        // For Node and Edge Function
        Eigen::Matrix<uint64_t, Eigen::Dynamic, Eigen::Dynamic> transfer_edge_mat_;
        UMap<uint64_t, uint64_t> node_2_mat_;
        UMap<uint64_t, uint64_t> action_2_mat_;

        friend class StateMachineSerializer;
    };
}