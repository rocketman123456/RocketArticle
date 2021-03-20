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
            draw_sub_passes_.push_back(CreateRef<Geometry2DSubPass>());
            draw_sub_passes_.push_back(CreateRef<GeometrySubPass>());
            draw_sub_passes_.push_back(CreateRef<SkyBoxSubPass>());
            draw_sub_passes_.push_back(CreateRef<GuiSubPass>());
        }
    };
}