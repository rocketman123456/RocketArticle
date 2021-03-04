#pragma once
#include "Core/Core.h"
#include "Interface/IRuntimeModule.h"

namespace Rocket
{
    class GameLogic : implements IRuntimeModule
    {
    public:
        RUNTIME_MODULE_TYPE(AssetLoader);
        virtual ~GameLogic() = default;

        [[nodiscard]] int Initialize() final;
        void Finalize() final;

        void Tick(Timestep ts) final;
    };
}