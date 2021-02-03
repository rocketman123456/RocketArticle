#pragma once
#include "Render/DrawSubPass/BaseSubPass.h"

namespace Rocket
{
    class Geometry2DSubPass : implements BaseSubPass
    {
    public:
        void Draw(Frame& frame) final;
    };
}