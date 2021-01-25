#pragma once
#include "Interface/ISubPass.h"
#include "Render/FrameStructure.h"

namespace Rocket
{
    Interface IDrawSubPass : inheritance ISubPass
    {
    public:
        virtual void Draw(Frame& frame) = 0;
    };
}