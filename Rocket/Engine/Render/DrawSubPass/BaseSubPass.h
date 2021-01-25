#pragma once
#include "Interface/IDrawSubPass.h"

namespace Rocket
{
    class BaseSubPass : implements IDrawSubPass {
    public:
        void BeginSubPass() override{};
        void EndSubPass() override{};
    };
}