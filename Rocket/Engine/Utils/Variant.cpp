#include "Utils/Variant.h"
#include "Module/MemoryManager.h"

using namespace Rocket;

void* Variant::operator new(std::size_t sz)
{
    return g_MemoryManager->Allocate(sz);
}

void* Variant::operator new[](std::size_t sz)
{
    return g_MemoryManager->Allocate(sz);
}

void Variant::operator delete(void* ptr, std::size_t sz)
{
    g_MemoryManager->Free(ptr, sz);
}

void Variant::operator delete[](void* ptr, std::size_t sz)
{
    g_MemoryManager->Free(ptr, sz);
}
