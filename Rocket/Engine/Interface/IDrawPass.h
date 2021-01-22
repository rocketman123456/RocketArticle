#pragma once
#include "Interface/IPass.h"
#include "Render/FrameStructure.h"

namespace Rocket
{
    Interface IDrawPass : inheritance IPass
    {
    public:
        virtual void Draw(Frame& frame) = 0;
    };
}