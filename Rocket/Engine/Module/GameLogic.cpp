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
}

bool GameLogic::OnUIEvent(EventPtr& e)
{
    m_StateMachine->Update(e->GetStringId(1), e->Var);
    return false;
}
