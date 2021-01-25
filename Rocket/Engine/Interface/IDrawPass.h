#pragma once
#include "Interface/IPass.h"
#include "Render/FrameStructure.h"

namespace Rocket
{
    Interface IDrawPass : inheritance IPass
    {
    public:
        IDrawPass() = default;
        virtual ~IDrawPass() = default;

        virtual void BeginPass(Frame & frame) = 0;
        virtual void Draw(Frame & frame) = 0;
        virtual void EndPass(Frame & frame) = 0;
    };
}