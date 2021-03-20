#include "Utils/Hashing.h"

using namespace Rocket;

static const uint64_t _HashString_(const String& str, UMap<uint64_t, String>& id_map, int type = 3)
{
    uint64_t result = 0;
    if(type == 1)
        for (auto it = str.cbegin(); it != str.cend(); ++it) 
            result = (result * 131) + *it;
    else if(type == 2)
        result = std::hash<String>()(str);
    else if(type == 3)
        result = std::hash<String>{}(str);
    else if(type == 4)
        result = Rocket::hash(str);

    auto it = id_map.find(result);
    if (it != id_map.end())
    {
        if (it->second == str)
        {
            return result;
        }
        else
        {
            RK_CORE_ERROR("Hash String [{}] [{}] Collide", it->second, str);
            return 0;
        }
    }
    else
    {
        id_map[result] = str;
        return result;
    }
}

static const String& _GetStringFromId_(uint64_t id, UMap<uint64_t, String>& id_map)
{
    static String empty = "";
    auto it = id_map.find(id);
    if (it != id_map.end())
    {
        return it->second;
    }
    else
    {
        RK_CORE_WARN("Hash String Table Don't Have String ID {}", id);
        return empty;
    }
}

UMap<uint64_t, UMap<uint64_t, String>> GlobalHashTable::id_string_map;

uint64_t GlobalHashTable::HashString(const uint64_t type_id, const String& str)
{
    auto it = id_string_map.find(type_id);
    if(it != id_string_map.end())
    {
        auto result = _HashString_(str, it->second); 
        return result; 
    }
    else
    {
        id_string_map[type_id] = {};
        auto result = _HashString_(str, id_string_map[type_id]);
        return result; 
    }
}

const String& GlobalHashTable::GetStringFromId(const uint64_t type_id, uint64_t id)
{
    static String empty = "";
    auto it = id_string_map.find(type_id);
    if(it != id_string_map.end())
    {
        auto& result = _GetStringFromId_(id, it->second); 
        return result; 
    }
    else
    {
        RK_CORE_WARN("Hash String Table Don't Have String ID {}", id);
        return empty;
    }
}
