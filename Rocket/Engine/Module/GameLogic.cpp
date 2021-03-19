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
    m_StateMachine.reset();
}

void GameLogic::Tick(Timestep ts)
{
    // Update state machine to finish state transfer
    bool result = m_StateMachine->GetTransferFinish();
    if(!result && m_CurrentInputData.size() > 0)
    {
        // Update Action
        m_StateMachine->UpdateAction(m_CurrentInputData);
    }
    else if(result && m_PendingStateData.size() > 0)
    {
        result = m_StateMachine->UpdateEdge(m_PendingStateData);
        m_PendingStateData.clear();
    }
}

bool GameLogic::OnResponseEvent(EventPtr& e)
{
    RK_CORE_TRACE("Set Input Data");
    m_CurrentInputData.assign(e->Var.begin(), e->Var.end());
    m_StateMachine->UpdateAction(m_CurrentInputData);
    m_CurrentInputData.clear();
    return false;
}

bool GameLogic::OnUIEvent(EventPtr& e)
{
    bool result = m_StateMachine->GetTransferFinish();
    if(!result)
    {
        RK_EVENT_TRACE("UI Event {} - StateMachine is in Transfer", 
            GlobalHashTable::GetStringFromId("Event"_hash, e->Var[0].asStringId)
        );
        if(m_PendingStateData.size() == 0)
            m_PendingStateData.assign(e->Var.begin(), e->Var.end());
        return false;
    }
    m_PendingStateData.clear();
    result = m_StateMachine->UpdateEdge(e->Var);
    m_CurrentStateData.assign(e->Var.begin(), e->Var.end());
    return false;
}
