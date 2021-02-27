#include "Render/DispatchPass/SkyBoxGenerate.h"
#include "Module/GraphicsManager.h"

using namespace Rocket;

void SkyBoxGenerate::Dispatch(Frame& frame)
{
    g_GraphicsManager->GenerateCubeMaps();
}