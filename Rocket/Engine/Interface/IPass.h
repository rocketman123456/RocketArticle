#pragma once
#include "Core/Core.h"

namespace Rocket
{
    Interface IPass
    {
    public:
        virtual void BeginPass() = 0;
        virtual void EndPass() = 0;
    };
}