#include "Logic/StateMachine.h"
//#include "Module/EventManager.h"
#include "Utils/GenerateName.h"
#include "Utils/Hashing.h"

using namespace Rocket;

StateNode::StateNode(const String& _name) : name(_name) { id = StateMachineHashTable::HashString(name); }
StateEdge::StateEdge(const String& _name) : name(_name) { id = StateMachineHashTable::HashString(name); }

void StateNode::AddEgde(const Ref<StateEdge>& edge)
{
    edgeList[edge->id] = edge;
}

Ref<StateEdge> StateNode::GetEdge(uint64_t id)
{
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

bool StateMachine::UpdateAction(const Vec<Variant>& data)
{
    RK_CORE_TRACE("Update Along Edge {}", currentEdge->name);
    bool result = currentEdge->actionFun(data, currentEdge->data);
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

bool StateMachine::Update(const Vec<Variant>& data)
{
    RK_CORE_TRACE("State Machine Update on Action {}", StateMachineHashTable::GetStringFromId(data[1].asStringId));
    // Step 1
    // Get Edge to Follow
    if(!currentEdge)
    {
        auto edge_id = currStateNode->transferFun(data, currentState);
        currentEdge = currStateNode->GetEdge(edge_id);
        if(!currentEdge)
        {
            return false;
        }
        // Set Edge Data
        currentEdge->finished = false;
        currentEdge->data.assign(data.begin(), data.end());
        RK_CORE_TRACE("Get New Edge {}", currentEdge->name);
    }
    else
    {
        RK_CORE_TRACE("Keep Update Along Edge {}", currentEdge->name);
        return false;
    }
    // Step 2
    // Take Edge Action
    if(!currentEdge->finished)
    {
        bool result = UpdateAction(data);
        return result;
    }
    else
    {
        RK_CORE_TRACE("Edge Action Unfinish {}", currentEdge->name);
        return false;
    }
}