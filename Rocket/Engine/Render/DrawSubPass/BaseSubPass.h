#pragma once
#include "Interface/IDrawSubPass.h"

namespace Rocket
{
    Interface BaseSubPass : inheritance IDrawSubPass
    {
    public:
        void BeginSubPass() override {};
        void EndSubPass() override {};
    };
}