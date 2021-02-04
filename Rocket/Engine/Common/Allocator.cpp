#include "Common/Allocator.h"

using namespace Rocket;

Allocator::Allocator(size_t size) : _size(size), _used_memory(0), _num_allocations(0)
{
	RK_CORE_ASSERT(size > 0, "");
}

Allocator::~Allocator()
{
	RK_CORE_ASSERT(_num_allocations == 0 && _used_memory == 0, "");
}

size_t Allocator::getSize() const
{
	return _size;
}

size_t Allocator::getUsedMemory() const
{
	return _used_memory;
}

size_t Allocator::getNumAllocations() const
{
	return _num_allocations;
}