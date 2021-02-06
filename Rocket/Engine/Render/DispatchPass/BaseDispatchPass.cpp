#include "Render/DispatchPass/BaseDispatchPass.h"
#include "Module/GraphicsManager.h"

using namespace Rocket;

void BaseDispatchPass::BeginPass([[maybe_unused]] Frame& frame)
{
    g_GraphicsManager->BeginCompute(); 
}

void BaseDispatchPass::EndPass([[maybe_unused]] Frame& frame)
{
    g_GraphicsManager->EndCompute(); 
}
