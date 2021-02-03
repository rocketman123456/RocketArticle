#pragma once
#include "Core/Core.h"
#include <cstdlib>
#include <new>
#include <memory>
#include "Module/MemoryManager.h"

namespace Rocket
{
    template <class T>
    struct Mallocator
    {
        typedef T value_type;
        Mallocator() noexcept {} //default ctor not required by C++ Standard Library

        // A converting copy constructor:
        template <class U>
        Mallocator(const Mallocator<U> &) noexcept {}
        template <class U>
        bool operator==(const Mallocator<U> &) const noexcept
        {
            return true;
        }
        template <class U>
        bool operator!=(const Mallocator<U> &) const noexcept
        {
            return false;
        }
        T *allocate(const size_t n) const;
        void deallocate(T *const p, size_t n) const noexcept;
    };

    template <class T>
    T *Mallocator<T>::allocate(const size_t n) const
    {
        if (n == 0)
        {
            return nullptr;
        }
        if (n > static_cast<size_t>(-1) / sizeof(T))
        {
            throw std::bad_array_new_length();
        }
        void *const pv = malloc(n * sizeof(T));
        if (!pv)
        {
            throw std::bad_alloc();
        }
        return static_cast<T *>(pv);
    }

    template <class T>
    void Mallocator<T>::deallocate(T *const p, size_t n) const noexcept
    {
        free(p);
    }
} // namespace Rocket
