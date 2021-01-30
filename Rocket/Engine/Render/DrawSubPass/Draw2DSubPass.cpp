#include "Render/DrawSubPass/Draw2DSubPass.h"
#include "Module/PipelineStateManager.h"
#include "Module/GraphicsManager.h"

namespace Rocket
{
    void Draw2DSubPass::Draw(Frame& frame)
    {
        auto& pPipelineState = g_PipelineStateManager->GetPipelineState("Draw2D");

        // Set the color shader as the current shader program and set the matrices
        // that it will use for rendering.
        g_GraphicsManager->SetPipelineState(pPipelineState, frame);
        g_GraphicsManager->DrawBatch(frame);
    }
}