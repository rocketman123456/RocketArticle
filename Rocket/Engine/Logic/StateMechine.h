#pragma once
#include "Core/Core.h"

#include <functional>

namespace Rocket
{
    struct State
    {
        State* pParent;
        State* pChild;
        std::function<bool(int)> TransferFun;
    };
}