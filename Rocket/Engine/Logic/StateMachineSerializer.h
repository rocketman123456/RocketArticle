#pragma once
#include "Core/Core.h"
#include "StateMachine.h"

#include <yaml-cpp/yaml.h>

namespace Rocket
{
    class StateMachineSerializer
    {
    public:
        StateMachineSerializer() = default;
        ~StateMachineSerializer() = default;
    public:
        static Ref<StateMachine> Deserialize(const String& name, TransferFunction transfer, ActionFunction action);
        static void Serialize(const String& name, Ref<StateMachine> stateMachine);
    };
}