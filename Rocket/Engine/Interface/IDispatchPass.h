#pragma once
#include "Interface/IPass.h"
#include "Render/FrameStructure.h"

namespace Rocket
{
    Interface IDispatchPass : inheritance IPass
    {
    public:
        virtual void Dispatch(Frame& frame) = 0;
    };
}