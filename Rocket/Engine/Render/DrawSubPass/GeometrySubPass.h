#pragma once
#include "Render/DrawSubPass/BaseSubPass.h"

namespace Rocket
{
    class GeometrySubPass : implements BaseSubPass
    {
    public:
        void Draw(Frame& frame) final;
    };
}