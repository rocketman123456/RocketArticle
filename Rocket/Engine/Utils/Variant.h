#pragma once
#include <variant>

namespace Rocket
{
    struct Variant
    {
        enum Type
        {
            TYPE_INTEGER = 0,
            TYPE_FLOAT,
            TYPE_BOOL,
            TYPE_STRING_ID,
            TYPE_COUNT,
        };
        Type type;
        std::variant<int32_t, float, bool, uint32_t> var;
    };
}