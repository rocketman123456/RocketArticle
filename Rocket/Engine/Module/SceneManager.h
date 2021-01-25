#pragma once
#include "Interface/IRuntimeModule.h"

namespace Rocket
{
    class SceneManager : implements IRuntimeModule
    {
    public:
        RUNTIME_MODULE_TYPE(SceneManager);
        SceneManager() = default;
        virtual ~SceneManager() = default;
    };

    SceneManager* GetSceneManager();
    extern SceneManager* g_SceneManager;
}