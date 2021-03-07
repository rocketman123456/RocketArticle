#include "Logic/StateMachine.h"
#include "Utils/GenerateName.h"
#include "Utils/Hashing.h"

using namespace Rocket;

StateNode::StateNode(const String& _name)
    : name(_name)
{
    id = HashFunction::Hash<String>(name);
}

StateEdge::StateEdge(const String& _name)
    : name(_name)
{
    id = HashFunction::Hash<String>(name);
}

void StateMachine::SetInitState(Ref<StateNode> init)
{
    initStateNode = init;
    currStateNode = init;
}

void StateMachine::ResetToInitState()
{
    currStateNode = initStateNode;
}

void StateMachine::Update(uint64_t action, const Vec<Variant>& data)
{
    RK_CORE_TRACE("State Machine Update on Action {}", action);
    // Step 1
    // Get Edge to Follow
    if(!currentEdge)
    {
        currentEdge = currStateNode->transferFun(action, currentState);
        if(!currentEdge)
        {
            return;
        }
        RK_CORE_TRACE("Get New Edge {}", currentEdge->name);
    }
    else
    {
        RK_CORE_TRACE("Keep Update Along Edge {}", currentEdge->name);
    }
    // Step 2
    // Take Edge Action
    if(!currentEdge->finished)
    {
        RK_CORE_TRACE("Update Along Edge {}", currentEdge->name);
        bool result = currentEdge->actionFun(data);
        currentEdge->finished = result;
        // Check Action Result
        if(currentEdge->finished)
        {
            RK_CORE_TRACE("Update To State {}", currentEdge->child->name);
            currStateNode = currentEdge->child;
            currentEdge = nullptr;
        }
    }
    else
    {
        RK_CORE_TRACE("Edge Action Unfinish {}", currentEdge->name);
    }
}