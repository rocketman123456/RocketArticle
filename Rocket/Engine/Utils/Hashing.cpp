#include "Utils/Hashing.h"

namespace Rocket
{
    UMap<uint64_t, std::string> EventHashTable::IdStringMap;
    UMap<uint64_t, std::string> AssetHashTable::IdStringMap;

    uint64_t EventHashTable::HashString(const std::string& str)
    {
        uint64_t result = std::hash<std::string>{}(str);
        auto it = IdStringMap.find(result);
        if(it != IdStringMap.end())
        {
            if(it->second == str)
            {
                return result;
            }
            else
            {
                RK_CORE_ERROR("Hash String [{}] [{}] Collide", it->second, str);
                return 0;
            }
        }
        IdStringMap[result] = str;
        return result;
    }

    uint64_t AssetHashTable::HashString(const std::string& str)
    {
        uint64_t result = std::hash<std::string>{}(str);
        auto it = IdStringMap.find(result);
        if(it != IdStringMap.end())
        {
            if(it->second == str)
            {
                return result;
            }
            else
            {
                RK_CORE_ERROR("Hash String [{}] [{}] Collide", it->second, str);
                return 0;
            }
        }
        IdStringMap[result] = str;
        return result;
    }

    const std::string& EventHashTable::GetStringFromId(uint64_t id)
    {
        auto it = IdStringMap.find(id);
        if(it != IdStringMap.end())
        {
            return it->second;
        }
        RK_CORE_ASSERT(false, "Hash String Table Don't Have String ID");
    }

    const std::string& AssetHashTable::GetStringFromId(uint64_t id)
    {
        auto it = IdStringMap.find(id);
        if(it != IdStringMap.end())
        {
            return it->second;
        }
        RK_CORE_ASSERT(false, "Hash String Table Don't Have String ID");
    }
}