#pragma once
#include "Render/DrawPass/BaseDrawPass.h"
#include "Render/DrawSubPass/Geometry2DSubPass.h"
#include "Render/DrawSubPass/GeometrySubPass.h"
#include "Render/DrawSubPass/GuiSubPass.h"

namespace Rocket
{
    class ForwardGeometryPass : implements BaseDrawPass
    {
    public:
        ForwardGeometryPass()
        {
            //m_DrawSubPasses.push_back(std::make_shared<GeometrySubPass>());
            m_DrawSubPasses.push_back(std::make_shared<Geometry2DSubPass>());
            //m_DrawSubPasses.push_back(std::make_shared<SkyBoxSubPass>());
            m_DrawSubPasses.push_back(std::make_shared<GuiSubPass>());
        }
    };
}