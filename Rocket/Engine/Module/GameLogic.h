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

        void SetStateMachine(const Ref<StateMachine>& stateMachine) { m_StateMachine = stateMachine; }
        Ref<StateMachine> GetStateMachine() { return m_StateMachine; }

        bool OnUIEvent(EventPtr& e);
        bool OnResponseEvent(EventPtr& e);

    private:
        Ref<StateMachine> m_StateMachine = nullptr;
        Vec<Variant> m_CurrentStateData;
        Vec<Variant> m_CurrentInputData;
        Vec<Variant> m_PendingStateData;
    };

    GameLogic* GetGameLogic();
    extern GameLogic* g_GameLogic;
}