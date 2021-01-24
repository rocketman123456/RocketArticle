#pragma once
#include "Interface/IRuntimeModule.h"

namespace Rocket
{
    class MemoryManager : implements IRuntimeModule
    {
    public:
        RUNTIME_MODULE_TYPE(MemoryManager);
        MemoryManager() = default;
        virtual ~MemoryManager() = default;
    };

    MemoryManager* GetMemoryManager();
    extern MemoryManager* g_MemoryManager;
}