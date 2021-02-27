#pragma once
#include "Render/DrawSubPass/BaseSubPass.h"

namespace Rocket
{
    class SkyBoxSubPass : implements BaseSubPass
    {
    public:
        void Draw(Frame& frame) final;
    };
}