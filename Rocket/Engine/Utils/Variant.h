#pragma once
#include <variant>

namespace Rocket
{
    using string_id = uint64_t;
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
        std::variant<
            int32_t,
            float,
            bool,
            string_id>
            var;

        auto GetIndex()
        {
            return var.index();
        }
    };
} // namespace Rocket