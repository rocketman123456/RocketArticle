#include "Logic/StateMachine.h"

using namespace Rocket;

void StateMechine::SetInitState(Ref<StateNode> init)
{
    initStateNode = init;
    currStateNode = init;
}

void StateMechine::ResetToInitState()
{
    currStateNode = initStateNode;
}

void StateMechine::Update(uint64_t action)
{
    auto edge = currStateNode->transferFun(action, currentState);
}