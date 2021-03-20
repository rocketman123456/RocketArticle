#pragma once
#include "Render/DrawSubPass/BaseSubPass.h"
#include "Utils/Timer.h"

namespace Rocket
{
    class GuiSubPass : implements BaseSubPass
    {
    public:
        GuiSubPass() { timer_.Start(); }
        virtual ~GuiSubPass() = default;
        void Draw(Frame& frame) final;
    private:
        double count_time_ = 0.0f;
        int32_t count_fps_ = 0;
        int32_t fps_ = 0;
        ElapseTimer timer_;
    };
}