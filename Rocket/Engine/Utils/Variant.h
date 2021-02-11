#pragma once
#include "Core/Core.h"
#include <variant>

namespace Rocket
{
    using string_id = uint64_t;
    struct Variant
    {
        enum Type
        {
            TYPE_INT32 = 0,
            TYPE_UINT32,
            TYPE_FLOAT,
            TYPE_DOUBLE,
            TYPE_BOOL,
            TYPE_POINTER,
            TYPE_STRING_ID,
            TYPE_COUNT,
        };
        Type type;
        union
        {
            int32_t m_asInt32;
            uint32_t m_asUInt32;
            float m_asFloat;
            double m_asDouble;
            bool m_asBool;
            void* m_asPointer;
            string_id m_asStringId;
        };

        //static void* operator new(std::size_t sz);
        //static void* operator new[](std::size_t sz);
        //static void operator delete(void* ptr, std::size_t sz);
        //static void operator delete[](void* ptr, std::size_t sz);
    };
} // namespace Rocket