#include "Utils/Hashing.h"

namespace Rocket
{
    static uint64_t _HashString_(const std::string& str, UMap<uint64_t, std::string>& id_map)
    {
        uint64_t result = std::hash<std::string>{}(str);
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
        id_map[result] = str;
        return result;
    }

    static const std::string& _GetStringFromId_(uint64_t id, UMap<uint64_t, std::string>& id_map)
    {
        auto it = id_map.find(id);
        if (it != id_map.end())
        {
            return it->second;
        }
        RK_CORE_ASSERT(false, "Hash String Table Don't Have String ID");
    }

    ImplementHashTable(EventHashTable);
    ImplementHashTable(AssetHashTable);
    ImplementHashTable(GraphicsHashTable);
}