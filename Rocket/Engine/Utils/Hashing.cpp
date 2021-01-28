#include "Utils/Hashing.h"

namespace Rocket
{
    std::unordered_map<std::string, uint64_t> HashTable::StringIdMap;
    std::unordered_map<uint64_t, std::string> HashTable::IdStringMap;
    //std::unordered_map<std::string, uint64_t> EventHashTable::StringIdMap;
    //std::unordered_map<uint64_t, std::string> EventHashTable::IdStringMap;
    //std::unordered_map<std::string, uint64_t> AssetHashTable::StringIdMap;
    //std::unordered_map<uint64_t, std::string> AssetHashTable::IdStringMap;

    uint64_t HashTable::HashString(const std::string& str)
    {
        uint64_t result = std::hash<std::string>{}(str);
        auto it = StringIdMap.find(str);
        if(it != StringIdMap.end())
        {
            return it->second;
        }
        StringIdMap[str] = result;
        auto it_ = IdStringMap.find(result);
        if(it_ != IdStringMap.end())
        {
            RK_CORE_ERROR("Hash String [{}] Confilict at ID [{}]", str, result);
        }
        IdStringMap[result] = str;
        return result;
    }

    const std::string& HashTable::GetIdString(uint64_t id)
    {
        auto it = IdStringMap.find(id);
        if(it != IdStringMap.end())
        {
            return it->second;
        }
        RK_CORE_ASSET(false, "Hash String Table Don't Have String ID");
    }

    template<typename T>
    uint64_t HashTable::Hash(T content)
    {
        uint64_t result = std::hash<T>{}(content);
        return result;
    }
}