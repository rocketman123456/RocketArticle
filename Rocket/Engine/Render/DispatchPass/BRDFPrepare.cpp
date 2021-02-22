#include "Render/DispatchPass/BRDFPrepare.h"
#include "Module/GraphicsManager.h"

using namespace Rocket;

void BRDFPrepare::Dispatch(Frame& frame)
{
    g_GraphicsManager->GenerateBRDFLUT(512);
}