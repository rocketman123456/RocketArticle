#pragma once
#include "Interface/IRuntimeModule.h"

namespace Rocket
{
    Interface IMemoryManager : inheritance IRuntimeModule {
    public:
        IMemoryManager(const std::string& name) : IRuntimeModule(name) {}
        virtual ~IMemoryManager() = default;

        virtual void* AllocatePage(size_t size) = 0;
        virtual void FreePage(void* p) = 0;
    };

    extern IMemoryManager* g_pMemoryManager;
}