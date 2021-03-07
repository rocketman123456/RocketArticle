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

        explicit Variant() : type(Variant::TYPE_INT32), m_asInt32(0) {}
        explicit Variant(Type _type, string_id data) : type(_type), m_asStringId(data) {}
        explicit Variant(Type _type, int32_t data) : type(_type), m_asInt32(data) {}
        explicit Variant(Type _type, uint32_t data) : type(_type), m_asUInt32(data) {}
        explicit Variant(Type _type, float data) : type(_type), m_asFloat(data) {}
        explicit Variant(Type _type, double data) : type(_type), m_asDouble(data) {}
        explicit Variant(Type _type, bool data) : type(_type), m_asBool(data) {}
        explicit Variant(Type _type, void* data) : type(_type), m_asPointer(data) {}
        
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
    };
} // namespace Rocket