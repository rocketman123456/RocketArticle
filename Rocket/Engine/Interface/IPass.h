#pragma once
#include "Core/Core.h"
#include "Render/FrameStructure.h"

namespace Rocket
{
    Interface IPass
    {
    public:
        virtual void BeginPass(Frame& frame) = 0;
        virtual void EndPass(Frame& frame) = 0;
    };
}