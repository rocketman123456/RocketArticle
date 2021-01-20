#pragma once
#include "Interface/IRuntimeModule.h"

namespace Rocket
{
    class MemoryManager : implements IRuntimeModule
    {
    public:
        MemoryManager() : IRuntimeModule("MemoryManager") {}
        virtual ~MemoryManager() = default;
    };

    MemoryManager* GetMemoryManager();
    extern MemoryManager* g_MemoryManager;
}