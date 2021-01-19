#pragma once
#include "Interface/IRuntimeModule.h"

namespace Rocket
{
    class SceneManager : implements IRuntimeModule
    {
    public:
        SceneManager() : IRuntimeModule("SceneManager") {}
        virtual ~SceneManager() = default;
    };

    SceneManager* GetSceneManager();
    extern SceneManager* g_SceneManager;
}