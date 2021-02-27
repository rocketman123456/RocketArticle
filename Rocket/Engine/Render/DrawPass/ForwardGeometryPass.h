#pragma once
#include "Render/DrawPass/BaseDrawPass.h"
#include "Render/DrawSubPass/Geometry2DSubPass.h"
#include "Render/DrawSubPass/GeometrySubPass.h"
#include "Render/DrawSubPass/SkyBoxSubPass.h"
#include "Render/DrawSubPass/GuiSubPass.h"

namespace Rocket
{
    class ForwardGeometryPass : implements BaseDrawPass
    {
    public:
        ForwardGeometryPass()
        {
            m_DrawSubPasses.push_back(CreateRef<Geometry2DSubPass>());
            m_DrawSubPasses.push_back(CreateRef<GeometrySubPass>());
            m_DrawSubPasses.push_back(CreateRef<SkyBoxSubPass>());
            m_DrawSubPasses.push_back(CreateRef<GuiSubPass>());
        }
    };
}