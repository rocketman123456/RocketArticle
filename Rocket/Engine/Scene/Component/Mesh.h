#pragma once
#include "Scene/SceneComponent.h"

namespace Rocket
{
    Interface Mesh : implements SceneComponent
    {
    public:
        virtual ~Mesh() = default;
    };
}