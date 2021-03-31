#pragma once
#include "Core/Core.h"
#include "Interface/IRuntimeModule.h"
#include "Logic/StateMachine.h"
#include "Event/Event.h"

namespace Rocket
{
    class GameLogic : implements IRuntimeModule
    {
    public:
        RUNTIME_MODULE_TYPE(GameLogic);
        virtual ~GameLogic() = default;

        [[nodiscard]] int Initialize() final;
        void Finalize() final;

        void Tick(Timestep ts) final;

        void SetStateMachine(const Ref<StateMachine>& stateMachine) { state_machine_ = stateMachine; }
        Ref<StateMachine> GetStateMachine() { return state_machine_; }

        bool OnDataEvent(EventPtr& e);
        bool OnActionEvent(EventPtr& e);

    private:
        Ref<StateMachine> state_machine_ = nullptr;
        Vec<Variant> current_state_data_;
        Vec<Variant> current_input_data_;
        Vec<Variant> pending_state_data_;
    };

    GameLogic* GetGameLogic();
    extern GameLogic* g_GameLogic;
}