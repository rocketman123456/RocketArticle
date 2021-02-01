#pragma once
#include "Scene/SceneCamera.h"

namespace Rocket
{
    class EditorCamera : implements SceneCamera
    {
    public:
        EditorCamera() = default;
        virtual ~EditorCamera() = default;
    };
}