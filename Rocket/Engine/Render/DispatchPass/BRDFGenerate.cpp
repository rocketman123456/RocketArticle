#include "Render/DispatchPass/BRDFGenerate.h"
#include "Module/GraphicsManager.h"

using namespace Rocket;

void BRDFGenerate::Dispatch(Frame& frame)
{
    g_GraphicsManager->GenerateBRDFLUT(512);
}