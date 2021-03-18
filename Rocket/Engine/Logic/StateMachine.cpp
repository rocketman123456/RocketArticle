#include "Logic/StateMachine.h"
//#include "Module/EventManager.h"
#include "Utils/GenerateName.h"
#include "Utils/Hashing.h"

using namespace Rocket;

StateNode::StateNode(const String& _name) : name(_name) { id = GlobalHashTable::HashString("StateMachine"_hash, name); }
StateEdge::StateEdge(const String& _name) : name(_name) { id = GlobalHashTable::HashString("StateMachine"_hash, name); }

void StateNode::AddEgde(const Ref<StateEdge>& edge)
{
    edgeList[edge->id] = edge;
}

Ref<StateEdge> StateNode::GetEdge(uint64_t id)
{
#if defined(RK_DEBUG)
    for(auto it : edgeList)
    {
        RK_CORE_TRACE("Edge List {}:{}, address:{}", GlobalHashTable::GetStringFromId("StateMachine"_hash, it.first), it.first, (uint64_t)it.second.get());
    }
#endif

    auto it = edgeList.find(id);
    if(it != edgeList.end())
        return it->second;
    else
        return nullptr;
}

void StateMachine::SetInitState(Ref<StateNode> init)
{
    initStateNode = init;
    currStateNode = init;
    currentState = init->id;
}

void StateMachine::ResetToInitState()
{
    currStateNode = initStateNode;
}

bool StateMachine::UpdateAction(const Vec<Variant>& input)
{
    // Empty edge always finish action
    if(!currentEdge)
    {
        RK_CORE_TRACE("Empty Edge, No Action");
        return true;
    }
    RK_CORE_TRACE("Update Along Edge {}", currentEdge->name);
    bool result = currentEdge->actionFun(input, currentEdge->data);
    currentEdge->finished = result;
    isTransferFinish = result;
    // Check Action Result
    if(currentEdge->finished)
    {
        RK_CORE_TRACE("Update To State {}", currentEdge->child->name);
        currStateNode = currentEdge->child;
        currentState = currStateNode->id;
        currentEdge = nullptr;
    }
    return result;
}

bool StateMachine::UpdateEdge(const Vec<Variant>& target)
{
    RK_CORE_TRACE("State Machine Update on Action {}", GlobalHashTable::GetStringFromId("StateMachine"_hash, target[1].asStringId));
    // Get Edge to Follow
    if(!currentEdge)
    {
        uint64_t edge_id = currStateNode->transferFun(target, currentState);
        //RK_CORE_TRACE("Get Edge Id : {}", edge_id);
        currentEdge = currStateNode->GetEdge(edge_id);
        if(!currentEdge)
        {
            RK_CORE_TRACE("Get null Edge");
            return false;
        }
        // Init Edge Data
        isTransferFinish = false;
        currentEdge->finished = false;
        // target data
        currentEdge->data.assign(target.begin(), target.end());
        RK_CORE_TRACE("Get New Edge {}", currentEdge->name);
    }
    else
    {
        RK_CORE_TRACE("Keep Update Along Edge {}", currentEdge->name);
        return false;
    }
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

uint64_t StateMachine::update_along_mat(const Vec<Variant>& data, const uint64_t state)
{
    uint64_t action = data[1].asStringId;
    uint64_t action_id = Get2Mat(action_2_mat, action);
    uint64_t state_id = Get2Mat(node_2_mat, state);
    uint64_t edge_id = transfer_edge_mat(state_id, action_id);
    uint64_t edge = edge_id;

    RK_CORE_TRACE("state_id {}, action_id {}, edge_id {}", state_id, action_id, edge_id);
    RK_CORE_TRACE("Current State {}, Get Action {}, Along Edge {}", 
        GlobalHashTable::GetStringFromId("StateMachine"_hash, state),
        GlobalHashTable::GetStringFromId("StateMachine"_hash, action),
        GlobalHashTable::GetStringFromId("StateMachine"_hash, edge)
    );
    
    return edge;
}

bool StateMachine::is_near(float i, float j, float esp)
{
    return std::abs(i - j) <= esp;
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