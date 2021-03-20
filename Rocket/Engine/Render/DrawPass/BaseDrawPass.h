#pragma once
#include "Module/GraphicsManager.h"
#include "Interface/IDrawPass.h"
#include "Interface/IDrawSubPass.h"

namespace Rocket
{
    Interface BaseDrawPass : inheritance IDrawPass {
    public:
        void BeginPass(Frame& frame) override { g_GraphicsManager->BeginPass(frame); }
        void Draw(Frame& frame) override;
        void EndPass(Frame& frame) override { g_GraphicsManager->EndPass(frame); }

    protected:
        BaseDrawPass() = default;

    protected:
        Vec<Ref<IDrawSubPass>> draw_sub_passes_;
    };
}  // namespace Rocket