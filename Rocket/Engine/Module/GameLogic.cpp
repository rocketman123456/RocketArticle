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
    state_machine_.reset();
}

void GameLogic::Tick(Timestep ts)
{
    // Update state machine to finish state transfer
    bool result = state_machine_->GetTransferFinish();
    if(!result && current_input_data_.size() > 0)
    {
        // Update Action
        state_machine_->UpdateActionData(current_input_data_);
    }
    else if(result && pending_state_data_.size() > 0)
    {
        result = state_machine_->UpdateStateEdge(pending_state_data_);
        pending_state_data_.clear();
    }
}

bool GameLogic::OnDataEvent(EventPtr& e)
{
    //RK_CORE_TRACE("Set Input Data");
    current_input_data_.assign(e->variable.begin(), e->variable.end());
    state_machine_->UpdateActionData(current_input_data_);
    current_input_data_.clear();
    return false;
}

bool GameLogic::OnActionEvent(EventPtr& e)
{
    bool result = state_machine_->GetTransferFinish();
    if(!result)
    {
        RK_EVENT_TRACE("UI Event {} - StateMachine is in Transfer", 
            GlobalHashTable::GetStringFromId("Event"_hash, e->variable[0].asStringId)
        );
        if(pending_state_data_.size() == 0)
            pending_state_data_.assign(e->variable.begin(), e->variable.end());
        return false;
    }
    pending_state_data_.clear();
    result = state_machine_->UpdateStateEdge(e->variable);
    current_state_data_.assign(e->variable.begin(), e->variable.end());
    return false;
}
