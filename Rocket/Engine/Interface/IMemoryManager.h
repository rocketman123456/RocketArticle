#pragma once
#include "Interface/IRuntimeModule.h"

namespace Rocket
{
    Interface IMemoryManager : inheritance IRuntimeModule
    {
    public:
        IMemoryManager() = default;
        virtual ~IMemoryManager() = default;

        virtual void* AllocatePage(size_t size) = 0;
        virtual void FreePage(void* p) = 0;
    };
}