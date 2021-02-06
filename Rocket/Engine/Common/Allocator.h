#pragma once
// From Tutorial
// http://allenchou.net/2013/05/memory-management-part-1-of-3-stl-compatible-allocators/
// http://allenchou.net/2013/05/memory-management-part-2-of-3-stl-compatible-allocators/
// http://allenchou.net/2013/05/memory-management-part-3-of-3-stl-compatible-allocators/

#include "Module/MemoryManager.h"

namespace Rocket
{
	template<typename T>
	class Allocator
	{
	public:
        typedef T value_type;
        typedef T* pointer;
        typedef T& reference;
        typedef const T* const_pointer;
        typedef const T& const_reference;
        typedef unsigned size_type;
        typedef unsigned difference_type;

        template <typename U>
        struct rebind
        {
            typedef Allocator<U> other;
        };

        pointer allocate(unsigned n)
        {
            return reinterpret_cast<T*>(g_MemoryManager->Allocate(sizeof(T) * n));
        }

        void deallocate(pointer p, unsigned n)
        {
            g_MemoryManager->Free(p, sizeof(T) * n);
        }

        void construct(pointer p, const_reference clone)
        {
            new (p) T(clone);
        }

        void destroy(pointer p)
        {
            p->~T();
        }

        pointer address(reference x) const
        {
            return &x;
        }

        const_pointer address(const_reference x) const
        {
            return &x;
        }

        bool operator==(const Allocator& rhs)
        {
            return true;
        }

        bool operator!=(const Allocator& rhs)
        {
            return !operator==(rhs);
        }
	};
}