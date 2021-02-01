#include "Render/DrawSubPass/GeometrySubPass.h"
#include "Module/PipelineStateManager.h"
#include "Module/GraphicsManager.h"

namespace Rocket
{
    void GeometrySubPass::Draw(Frame& frame)
    {
        auto& pPipelineState = g_PipelineStateManager->GetPipelineState("PBR");

        //// Set the color shader as the current shader program and set the matrices
        //// that it will use for rendering.
        g_GraphicsManager->SetPipelineState(pPipelineState, frame);
        //g_GraphicsManager->SetShadowMaps(frame);
        g_GraphicsManager->DrawBatch(frame);
    }
}