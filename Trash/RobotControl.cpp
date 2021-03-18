#include "RobotControl.h"
#include "Utils/Hashing.h"
#include "Common/GeomMath.h"

using namespace Rocket;

extern Eigen::Matrix<uint64_t, Eigen::Dynamic, Eigen::Dynamic> transfer_edge_mat;
extern UMap<uint64_t, uint64_t> node_2_mat;
extern UMap<uint64_t, uint64_t> action_2_mat;

static uint64_t Get2Mat(const UMap<uint64_t, uint64_t>& map, uint64_t input)
{
    auto it = map.find(input);
    if(it != map.end())
        return it->second;
    RK_CORE_ASSERT(false, "cannot find id in mat");
    return 0;
}

static uint64_t Get2Map(const UMap<uint64_t, uint64_t>& map, uint64_t input)
{
    for(auto it : map)
        if(it.second == input)
            return it.first;
    return UINT_MAX;
}

uint64_t update_along_mat(const Vec<Variant>& data, const uint64_t state)
{
    uint64_t action = data[1].asStringId;
    uint64_t action_id = Get2Mat(action_2_mat, action);
    uint64_t state_id = Get2Mat(node_2_mat, state);
    uint64_t edge_id = transfer_edge_mat(state_id, action_id);
    uint64_t edge = edge_id;//Get2Map(edge_2_mat, edge_id);

    RK_CORE_TRACE("state_id {}, action_id {}, edge_id {}", state_id, action_id, edge_id);
    RK_CORE_TRACE("Current State {}, Get Action {}, Along Edge {}", 
        GlobalHashTable::GetStringFromId("StateMachine"_hash, state),
        GlobalHashTable::GetStringFromId("StateMachine"_hash, action),
        GlobalHashTable::GetStringFromId("StateMachine"_hash, edge)
    );
    
    return edge;
}

static bool is_near(float i, float j, float esp = 1e-3)
{
    return std::abs(i - j) <= esp;
}

bool action_on_edge(const Vec<Variant>& input, const Vec<Variant>& target)
{
    RK_CORE_ASSERT(input.size() == target.size(), "Data Size Unmatch");
    bool result = true;
    // Check Motor Data
    for(int i = 2; i < 2 + 10; ++i)
    {
        result = result & is_near(input[i].asFloat, target[i].asFloat);
    }
    // if finished, return true
    return result;
}
