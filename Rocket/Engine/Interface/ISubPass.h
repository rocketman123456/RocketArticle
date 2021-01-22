#pragma once
#include "Core/Core.h"

namespace Rocket
{
    Interface ISubPass
    {
    public:
        virtual void BeginSubPass() = 0;
        virtual void EndSubPass() = 0;
    };
}