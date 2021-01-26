#pragma once
#include "Interface/IDrawSubPass.h"

namespace Rocket
{
    class Draw2DSubPass : implements IDrawSubPass
    {
    public:
        void Draw(Frame& frame) final;
    };
}