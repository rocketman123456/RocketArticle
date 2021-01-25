#pragma once
#include "Render/DrawSubPass/BaseSubPass.h"

namespace Rocket
{
    class GuiSubPass : implements BaseSubPass
    {
    public:
        void Draw(Frame& frame) final;
    };
}