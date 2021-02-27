#include "Render/DrawPass/BaseDrawPass.h"

using namespace Rocket;

void BaseDrawPass::Draw(Frame& frame)
{
    for (const auto& pSubPass : m_DrawSubPasses)
    {
        pSubPass->BeginSubPass();
        pSubPass->Draw(frame);
        pSubPass->EndSubPass();
    }
}
