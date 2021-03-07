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

        explicit Variant() : type(Variant::TYPE_INT32), asInt32(0) {}
        explicit Variant(Type _type, string_id data) : type(_type), asStringId(data) {}
        explicit Variant(Type _type, int32_t data) : type(_type), asInt32(data) {}
        explicit Variant(Type _type, uint32_t data) : type(_type), asUInt32(data) {}
        explicit Variant(Type _type, float data) : type(_type), asFloat(data) {}
        explicit Variant(Type _type, double data) : type(_type), asDouble(data) {}
        explicit Variant(Type _type, bool data) : type(_type), asBool(data) {}
        explicit Variant(Type _type, void* data) : type(_type), asPointer(data) {}
        
        Type type;
        union
        {
            int32_t asInt32;
            uint32_t asUInt32;
            float asFloat;
            double asDouble;
            bool asBool;
            void* asPointer;
            string_id asStringId;
        };
    };
} // namespace Rocket