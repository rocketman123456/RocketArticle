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
    bool result = m_StateMachine->GetIsInTransfer();
    if(result)
    {
        m_StateMachine->Update(m_CurrentStateData);
    }
}

bool GameLogic::OnUIEvent(EventPtr& e)
{
    bool result = m_StateMachine->Update(e->Var);
    m_CurrentStateData.assign(e->Var.begin(), e->Var.end());
    return result;
}
