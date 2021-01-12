#pragma once
#include "Interface/IRuntimeModule.h"

namespace Rocket
{
    class GraphicsManager : inheritance IRuntimeModule
    {
    public:
        GraphicsManager(const std::string &name = "IRuntimeModule");
        virtual ~GraphicsManager() = default;
    };

    GraphicsManager* GetGraphicsManager();
    extern GraphicsManager* g_GraphicsManager;
}