#include "RobotControl.h"
#include "Utils/Hashing.h"
#include "Common/GeomMath.h"

using namespace Rocket;

static Eigen::Matrix<uint64_t, Eigen::Dynamic, Eigen::Dynamic> transfer_node_mat;
static Eigen::Matrix<uint64_t, Eigen::Dynamic, Eigen::Dynamic> transfer_edge_mat;
static UMap<uint64_t, uint64_t> node_2_mat;
static UMap<uint64_t, uint64_t> action_2_mat;
static UMap<uint64_t, uint64_t> edge_2_mat;

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

static void log_state_transfer(const uint64_t action, const uint64_t state)
{
    RK_CORE_TRACE("Current State {}, Get Action {}", 
        StateMachineHashTable::GetStringFromId(state),
        StateMachineHashTable::GetStringFromId(action)
    );
}

void initialize_variable()
{
    transfer_node_mat = Eigen::Matrix<uint64_t, Eigen::Dynamic, Eigen::Dynamic>(10, 3);
    transfer_edge_mat = Eigen::Matrix<uint64_t, Eigen::Dynamic, Eigen::Dynamic>(10, 3);

    // init
    transfer_node_mat(0, 0) = 0; transfer_node_mat(0, 1) = 1; transfer_node_mat(0, 2) = 4;
    // rot
    transfer_node_mat(1, 0) =          0; transfer_node_mat(1, 1) = 2; transfer_node_mat(1, 2) = UINT64_MAX;
    transfer_node_mat(2, 0) = UINT64_MAX; transfer_node_mat(2, 1) = 3; transfer_node_mat(2, 2) = UINT64_MAX;
    transfer_node_mat(3, 0) = UINT64_MAX; transfer_node_mat(3, 1) = 1; transfer_node_mat(3, 2) = UINT64_MAX;
    // move
    transfer_node_mat(4, 0) = 0; transfer_node_mat(4, 1) = UINT64_MAX; transfer_node_mat(4, 2) = 5;
    transfer_node_mat(5, 0) = 0; transfer_node_mat(5, 1) = UINT64_MAX; transfer_node_mat(5, 2) = 6;
    transfer_node_mat(6, 0) = 0; transfer_node_mat(6, 1) = UINT64_MAX; transfer_node_mat(6, 2) = 7;
    transfer_node_mat(7, 0) = 0; transfer_node_mat(7, 1) = UINT64_MAX; transfer_node_mat(7, 2) = 8;
    transfer_node_mat(8, 0) = 0; transfer_node_mat(8, 1) = UINT64_MAX; transfer_node_mat(8, 2) = 9;
    transfer_node_mat(9, 0) = 0; transfer_node_mat(9, 1) = UINT64_MAX; transfer_node_mat(9, 2) = 4;

    // init
    transfer_edge_mat(0, 0) = UINT64_MAX; transfer_edge_mat(0, 1) = 0; transfer_edge_mat(0, 2) = 1;
    // rot
    transfer_edge_mat(1, 0) =          2; transfer_edge_mat(1, 1) = 3; transfer_edge_mat(1, 2) = UINT64_MAX;
    transfer_edge_mat(2, 0) = UINT64_MAX; transfer_edge_mat(2, 1) = 4; transfer_edge_mat(2, 2) = UINT64_MAX;
    transfer_edge_mat(3, 0) = UINT64_MAX; transfer_edge_mat(3, 1) = 5; transfer_edge_mat(3, 2) = UINT64_MAX;
    // move
    transfer_edge_mat(4, 0) = 12; transfer_edge_mat(4, 1) = UINT64_MAX; transfer_edge_mat(4, 2) = 6;
    transfer_edge_mat(5, 0) = 13; transfer_edge_mat(5, 1) = UINT64_MAX; transfer_edge_mat(5, 2) = 7;
    transfer_edge_mat(6, 0) = 14; transfer_edge_mat(6, 1) = UINT64_MAX; transfer_edge_mat(6, 2) = 8;
    transfer_edge_mat(7, 0) = 15; transfer_edge_mat(7, 1) = UINT64_MAX; transfer_edge_mat(7, 2) = 9;
    transfer_edge_mat(8, 0) = 16; transfer_edge_mat(8, 1) = UINT64_MAX; transfer_edge_mat(8, 2) = 10;
    transfer_edge_mat(9, 0) = 17; transfer_edge_mat(9, 1) = UINT64_MAX; transfer_edge_mat(9, 2) = 11;

    // Action id
    action_2_mat[StateMachineHashTable::HashString("init_pos")] = 0;
    action_2_mat[StateMachineHashTable::HashString("rotation")] = 1;
    action_2_mat[StateMachineHashTable::HashString("walk")] = 2;

    // Edge id
    edge_2_mat[StateMachineHashTable::HashString("edge_01")] = 0;
    edge_2_mat[StateMachineHashTable::HashString("edge_04")] = 1;

    edge_2_mat[StateMachineHashTable::HashString("edge_10")] = 2;
    edge_2_mat[StateMachineHashTable::HashString("edge_12")] = 3;
    edge_2_mat[StateMachineHashTable::HashString("edge_23")] = 4;
    edge_2_mat[StateMachineHashTable::HashString("edge_31")] = 5;

    edge_2_mat[StateMachineHashTable::HashString("edge_45")] = 6;
    edge_2_mat[StateMachineHashTable::HashString("edge_56")] = 7;
    edge_2_mat[StateMachineHashTable::HashString("edge_67")] = 8;
    edge_2_mat[StateMachineHashTable::HashString("edge_78")] = 9;
    edge_2_mat[StateMachineHashTable::HashString("edge_89")] = 10;
    edge_2_mat[StateMachineHashTable::HashString("edge_94")] = 11;

    edge_2_mat[StateMachineHashTable::HashString("edge_40")] = 12;
    edge_2_mat[StateMachineHashTable::HashString("edge_50")] = 13;
    edge_2_mat[StateMachineHashTable::HashString("edge_60")] = 14;
    edge_2_mat[StateMachineHashTable::HashString("edge_70")] = 15;
    edge_2_mat[StateMachineHashTable::HashString("edge_80")] = 16;
    edge_2_mat[StateMachineHashTable::HashString("edge_90")] = 17;

    // Node id
    node_2_mat[StateMachineHashTable::HashString("init")] = 0;
    node_2_mat[StateMachineHashTable::HashString("rot_00")] = 1;
    node_2_mat[StateMachineHashTable::HashString("rot_01")] = 2;
    node_2_mat[StateMachineHashTable::HashString("rot_02")] = 3;
    node_2_mat[StateMachineHashTable::HashString("move_00")] = 4;
    node_2_mat[StateMachineHashTable::HashString("move_01")] = 5;
    node_2_mat[StateMachineHashTable::HashString("move_02")] = 6;
    node_2_mat[StateMachineHashTable::HashString("move_03")] = 7;
    node_2_mat[StateMachineHashTable::HashString("move_04")] = 8;
    node_2_mat[StateMachineHashTable::HashString("move_05")] = 9;
}

uint64_t update_along_mat(const Vec<Variant>& data, const uint64_t state)
{
    uint64_t action = data[1].asStringId;
    log_state_transfer(action, state);

    uint64_t action_id = Get2Mat(action_2_mat, action);
    uint64_t state_id = Get2Mat(node_2_mat, state);
    uint64_t edge_id = transfer_edge_mat(state_id, action_id);
    uint64_t edge = Get2Map(edge_2_mat, edge_id);

    return edge;
}

bool action_on_edge(const Vec<Variant>& input, const Vec<Variant>& target)
{
    RK_CORE_ASSERT(input.size() == target.size(), "Data Size Unmatch");
    // Check Motor Data
    for(int i = 2; i < 2 + 10; ++i)
    {

    }
    // if finished, return true
    return false;
}
