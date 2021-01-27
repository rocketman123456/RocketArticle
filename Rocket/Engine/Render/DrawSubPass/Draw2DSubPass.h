#pragma once
#include "Render/DrawSubPass/BaseSubPass.h"

namespace Rocket
{
    class Draw2DSubPass : implements BaseSubPass
    {
    public:
        void Draw(Frame& frame) final;
    };
}