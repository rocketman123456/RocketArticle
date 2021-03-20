#include "Render/DrawPass/BaseDrawPass.h"

using namespace Rocket;

void BaseDrawPass::Draw(Frame& frame)
{
    for (const auto& pSubPass : draw_sub_passes_)
    {
        pSubPass->BeginSubPass();
        pSubPass->Draw(frame);
        pSubPass->EndSubPass();
    }
}
