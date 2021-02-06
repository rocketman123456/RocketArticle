#pragma once
#include "Render/DrawSubPass/BaseSubPass.h"
#include "Utils/Timer.h"

namespace Rocket
{
    class GuiSubPass : implements BaseSubPass
    {
    public:
        GuiSubPass() { m_Timer.Start(); }
        void Draw(Frame& frame) final;
    private:
        float m_CountTime = 0.0f;
        int32_t m_CountFrame = 0;
        int32_t m_FPS = 0.0;
        ElapseTimer m_Timer;
    };
}