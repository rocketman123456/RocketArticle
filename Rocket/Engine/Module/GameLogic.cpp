#include "Module/GameLogic.h"

using namespace Rocket;

GameLogic* Rocket::GetGameLogic()
{
    return new GameLogic();
}

int GameLogic::Initialize()
{
    return 0;
}

void GameLogic::Finalize()
{
}

void GameLogic::Tick(Timestep ts)
{
    // Update state machine to finish state transfer
    bool result = m_StateMachine->GetTransferFinish();
    if(!result)
    {
        m_StateMachine->UpdateAction(m_CurrentStateData);
    }
    else if(m_PendingStateData.size() > 0)
    {
        result = m_StateMachine->Update(m_PendingStateData);
        m_PendingStateData.clear();
    }
}

bool GameLogic::OnUIEvent(EventPtr& e)
{
    bool result = m_StateMachine->GetTransferFinish();
    if(!result)
    {
        RK_EVENT_TRACE("UI Event {} - StateMachine is in Transfer", 
            EventHashTable::GetStringFromId(e->Var[0].asStringId)
        );
        m_PendingStateData.assign(e->Var.begin(), e->Var.end());
        return false;
    }
    m_PendingStateData.clear();
    result = m_StateMachine->Update(e->Var);
    m_CurrentStateData.assign(e->Var.begin(), e->Var.end());
    return result;
}
