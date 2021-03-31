#include "Logic/StateMachine.h"
//#include "Module/EventManager.h"
#include "Utils/GenerateName.h"
#include "Utils/Hashing.h"

using namespace Rocket;

StateNode::StateNode(const String& _name) : name(_name) { id = GlobalHashTable::HashString("StateMachine"_hash, name); }
StateEdge::StateEdge(const String& _name) : name(_name) { id = GlobalHashTable::HashString("StateMachine"_hash, name); }

void StateNode::AddEgde(const Ref<StateEdge>& edge)
{
    edge_list[edge->id] = edge;
}

Ref<StateEdge> StateNode::GetEdge(uint64_t id)
{
#if defined(RK_DEBUG)
    for(auto it : edge_list)
    {
        RK_CORE_TRACE("Edge List {}:{}, address:{}", GlobalHashTable::GetStringFromId("StateMachine"_hash, it.first), it.first, (uint64_t)it.second.get());
    }
#endif

    auto it = edge_list.find(id);
    if(it != edge_list.end())
        return it->second;
    else
        return nullptr;
}

void StateMachine::SetInitState(Ref<StateNode> init)
{
    init_state_node_ = init;
    curr_state_node_ = init;
    current_state_ = init->id;
}

void StateMachine::ResetToInitState()
{
    curr_state_node_ = init_state_node_;
}

bool StateMachine::UpdateActionData(const Vec<Variant>& input)
{
    // Empty edge always finish action
    if(!current_edge_)
    {
        //RK_CORE_TRACE("Empty Edge, No Action");
        return true;
    }
    RK_CORE_TRACE("Update Along Edge {}", current_edge_->name);
    bool result = current_edge_->action_function(input, current_edge_->data);
    current_edge_->finished = result;
    is_transfer_finish_ = result;
    // Check Action Result
    if(current_edge_->finished)
    {
        RK_CORE_TRACE("Update To State {}", current_edge_->child->name);
        curr_state_node_ = current_edge_->child;
        current_state_ = curr_state_node_->id;
        current_edge_ = nullptr;
    }
    return result;
}

bool StateMachine::UpdateStateEdge(const Vec<Variant>& target)
{
    RK_CORE_TRACE("State Machine Update on Action {}", GlobalHashTable::GetStringFromId("StateMachine"_hash, target[1].asStringId));
    // Get Edge to Follow
    if(!current_edge_)
    {
        uint64_t edge_id = curr_state_node_->transfer_function(target, current_state_);
        //RK_CORE_TRACE("Get Edge Id : {}", edge_id);
        current_edge_ = curr_state_node_->GetEdge(edge_id);
        if(!current_edge_)
        {
            RK_CORE_TRACE("Get null Edge");
            return false;
        }
        // Init Edge Data
        is_transfer_finish_ = false;
        current_edge_->finished = false;
        // target data
        current_edge_->data.assign(target.begin(), target.end());
        RK_CORE_TRACE("Get New Edge {}", current_edge_->name);
    }
    else
    {
        RK_CORE_TRACE("Keep Update Along Edge {}", current_edge_->name);
        return false;
    }
}

uint64_t StateMachine::update_along_mat(const Vec<Variant>& data, const uint64_t state)
{
    uint64_t action = data[1].asStringId;
    uint64_t action_id = Get2Mat(action_2_mat_, action);
    uint64_t state_id = Get2Mat(node_2_mat_, state);
    uint64_t edge_id = transfer_edge_mat_(state_id, action_id);
    uint64_t edge = edge_id;

    RK_CORE_TRACE("state_id {}, action_id {}, edge_id {}", state_id, action_id, edge_id);
    RK_CORE_TRACE("Current State {}, Get Action {}, Along Edge {}", 
        GlobalHashTable::GetStringFromId("StateMachine"_hash, state),
        GlobalHashTable::GetStringFromId("StateMachine"_hash, action),
        GlobalHashTable::GetStringFromId("StateMachine"_hash, edge)
    );
    
    return edge;
}

bool StateMachine::action_on_edge(const Vec<Variant>& input, const Vec<Variant>& target)
{
    RK_CORE_ASSERT(input.size() == target.size(), "Data Size Unmatch");
    bool result = true;
    // Check Motor Data
    for(int i = 2; i < 2 + 10; ++i)
    {
        result = result & is_near(input[i].asFloat, target[i].asFloat);
    }
    // if finished, return true
    return result;
}

bool StateMachine::is_near(float i, float j, float esp)
{
    return std::abs(i - j) <= esp;
}

uint64_t StateMachine::Get2Mat(const UMap<uint64_t, uint64_t>& map, uint64_t input)
{
    auto it = map.find(input);
    if(it != map.end())
        return it->second;
    RK_CORE_ASSERT(false, "cannot find id in mat");
    return 0;
}

uint64_t StateMachine::Get2Map(const UMap<uint64_t, uint64_t>& map, uint64_t input)
{
    for(auto it : map)
        if(it.second == input)
            return it.first;
    return UINT_MAX;
}