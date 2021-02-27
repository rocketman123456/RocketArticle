#include "Render/DrawSubPass/Geometry2DSubPass.h"
#include "Module/PipelineStateManager.h"
#include "Module/GraphicsManager.h"

using namespace Rocket;

void Geometry2DSubPass::Draw(Frame& frame)
{
    auto& pPipelineState = g_PipelineStateManager->GetPipelineState("Draw2D");

    // Set the color shader as the current shader program and set the matrices
    // that it will use for rendering.
    g_GraphicsManager->SetPipelineState(pPipelineState, frame);
    g_GraphicsManager->DrawBatch(frame);
}
