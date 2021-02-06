#include "Scene/SceneComponent.h"

#include <iostream>
#include <crossguid/guid.hpp>

using namespace xg;
using namespace std;
using namespace Rocket;

SceneComponent::SceneComponent()
{
    auto id = newGuid();
    m_Id = std::hash<Guid>{}(id);
}

ostream &operator<<(ostream &out, SceneComponentType type)
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
