#include "Utils/Hashing.h"

using namespace Rocket;

static uint64_t _HashString_(const String& str, UMap<uint64_t, String>& id_map)
{
    //uint64_t result = 0;
    // From Google Chrome
    //for (auto it = str.cbegin(); it != str.cend(); ++it) {
    //    result = (result * 131) + *it;
    //    //RK_CORE_TRACE("{}", *it);
    //}
    //uint64_t result = std::hash<std::string>()(str);
    //uint64_t result = std::hash<String>{}(str);
    uint64_t result = Rocket::hash(str);

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

static const String& _GetStringFromId_(uint64_t id, UMap<uint64_t, String>& id_map)
{
    auto it = id_map.find(id);
    if (it != id_map.end())
    {
        return it->second;
    }
    RK_CORE_WARN("Hash String Table Don't Have String ID {}", id);
    return "";
}

ImplementHashTable(StateMachineHashTable);
ImplementHashTable(EventHashTable);
ImplementHashTable(AssetHashTable);
ImplementHashTable(GraphicsHashTable);
ImplementHashTable(SceneHashTable);
