#pragma once
#include "Module/GraphicsManager.h"
#include "Interface/IDrawPass.h"
#include "Interface/IDrawSubPass.h"

namespace Rocket {
    class BaseDrawPass : implements IDrawPass {
    public:
        void BeginPass() override { g_GraphicsManager->BeginPass(); }
        void Draw(Frame& frame) override;
        void EndPass() override { g_GraphicsManager->EndPass(); }

    protected:
        BaseDrawPass() = default;

    protected:
        std::vector<Ref<IDrawSubPass>> m_DrawSubPasses;
    };
}  // namespace Rocket