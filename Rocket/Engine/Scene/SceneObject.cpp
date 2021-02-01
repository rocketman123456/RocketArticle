#include "Scene/SceneObject.h"
#include <iostream>

using namespace std;

namespace Rocket
{
    ostream &operator<<(ostream &out, SceneObjectType type)
    {
        auto n = static_cast<int32_t>(type);
        n = endian_net_unsigned_int<int32_t>(n);
        char *c = reinterpret_cast<char *>(&n);

        for (size_t i = 0; i < sizeof(int32_t); i++)
        {
            out << *c++;
        }

        return out;
    }

    ostream &operator<<(ostream &out, IndexDataType type)
    {
        auto n = static_cast<int32_t>(type);
        n = endian_net_unsigned_int<int32_t>(n);
        char *c = reinterpret_cast<char *>(&n);

        for (size_t i = 0; i < sizeof(int32_t); i++)
        {
            out << *c++;
        }

        return out;
    }

    ostream &operator<<(ostream &out, VertexDataType type)
    {
        auto n = static_cast<int32_t>(type);
        n = endian_net_unsigned_int<int32_t>(n);
        char *c = reinterpret_cast<char *>(&n);

        for (size_t i = 0; i < sizeof(int32_t); i++)
        {
            out << *c++;
        }

        return out;
    }

    ostream &operator<<(ostream &out, PrimitiveType type)
    {
        auto n = static_cast<int32_t>(type);
        n = endian_net_unsigned_int<int32_t>(n);
        char *c = reinterpret_cast<char *>(&n);

        for (size_t i = 0; i < sizeof(int32_t); i++)
        {
            out << *c++;
        }

        return out;
    }
} // namespace Rocket