#include "Render/DispatchPass/BaseDispatchPass.h"
#include "Module/GraphicsManager.h"

namespace Rocket
{
    void BaseDispatchPass::BeginPass([[maybe_unused]] Frame& frame) {
        g_pGraphicsManager->BeginCompute(); 
    }

    void BaseDispatchPass::EndPass([[maybe_unused]] Frame& frame) {
        g_pGraphicsManager->EndCompute(); 
    }
}