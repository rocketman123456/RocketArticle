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
        GlobalHashTable::GetStringFromId("StateMachine"_hash, state),
        GlobalHashTable::GetStringFromId("StateMachine"_hash, action)
    );
}

#define ACTION_MAT_ID(action) action_2_mat[GlobalHashTable::HashString("StateMachine"_hash, action)]
#define NODE_MAT_ID(node) node_2_mat[GlobalHashTable::HashString("StateMachine"_hash, node)]
#define EDGE_MAT_ID(edge) edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, edge)]

void initialize_variable()
{
    transfer_node_mat = Eigen::Matrix<uint64_t, Eigen::Dynamic, Eigen::Dynamic>(12, 3);
    transfer_edge_mat = Eigen::Matrix<uint64_t, Eigen::Dynamic, Eigen::Dynamic>(12, 3);

    // Action id
    action_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "init_pos")] = 0;
    action_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "rotation")] = 1;
    action_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "walk")] = 2;

    // Node id
    node_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "init")] = 0;
    node_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "rot_00")] = 1;
    node_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "rot_01")] = 2;
    node_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "rot_02")] = 3;
    node_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "move_00")] = 4;
    node_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "move_01")] = 5;
    node_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "move_02")] = 6;
    node_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "move_03")] = 7;
    node_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "move_04")] = 8;
    node_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "move_05")] = 9;
    node_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "rot_rec_0")] = 10;
    node_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "rot_rec_1")] = 11;

    // Edge id
    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_01")] = 0;
    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_04")] = 1;

    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_10")] = 2;
    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_12")] = 3;
    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_23")] = 4;
    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_32")] = 5;

    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_45")] = 6;
    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_56")] = 7;
    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_67")] = 8;
    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_78")] = 9;
    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_89")] = 10;
    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_94")] = 11;

    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_40")] = 12;
    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_50")] = 13;
    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_60")] = 14;
    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_70")] = 15;
    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_80")] = 16;
    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_90")] = 17;
    // rot_00 to init
    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_3_10")] = 18;
    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_10_11")] = 19;
    edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, "edge_11_1")] = 20;

    // init
    transfer_edge_mat(0, 0) = EDGE_MAT_ID("empty"); transfer_edge_mat(0, 1) = EDGE_MAT_ID("edge_01"); transfer_edge_mat(0, 2) = EDGE_MAT_ID("edge_04");
    // rot
    transfer_edge_mat(1, 0) = EDGE_MAT_ID("edge_10"); transfer_edge_mat(1, 1) = EDGE_MAT_ID("edge_12"); transfer_edge_mat(1, 2) = EDGE_MAT_ID("empty");
    transfer_edge_mat(2, 0) = EDGE_MAT_ID("empty"); transfer_edge_mat(2, 1) = EDGE_MAT_ID("edge_23"); transfer_edge_mat(2, 2) = EDGE_MAT_ID("empty");
    transfer_edge_mat(3, 0) = EDGE_MAT_ID("edge_3_10"); transfer_edge_mat(3, 1) = EDGE_MAT_ID("edge_32"); transfer_edge_mat(3, 2) = EDGE_MAT_ID("empty");
    // move
    transfer_edge_mat(4, 0) = EDGE_MAT_ID("edge_40"); transfer_edge_mat(4, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(4, 2) = EDGE_MAT_ID("edge_45");
    transfer_edge_mat(5, 0) = EDGE_MAT_ID("edge_50"); transfer_edge_mat(5, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(5, 2) = EDGE_MAT_ID("edge_56");
    transfer_edge_mat(6, 0) = EDGE_MAT_ID("edge_60"); transfer_edge_mat(6, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(6, 2) = EDGE_MAT_ID("edge_67");
    transfer_edge_mat(7, 0) = EDGE_MAT_ID("edge_70"); transfer_edge_mat(7, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(7, 2) = EDGE_MAT_ID("edge_78");
    transfer_edge_mat(8, 0) = EDGE_MAT_ID("edge_80"); transfer_edge_mat(8, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(8, 2) = EDGE_MAT_ID("edge_89");
    transfer_edge_mat(9, 0) = EDGE_MAT_ID("edge_90"); transfer_edge_mat(9, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(9, 2) = EDGE_MAT_ID("edge_94");
    // rot_rec
    transfer_edge_mat(10, 0) = EDGE_MAT_ID("edge_10_11"); transfer_edge_mat(10, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(10, 2) = EDGE_MAT_ID("empty");
    transfer_edge_mat(11, 0) = EDGE_MAT_ID("edge_11_1"); transfer_edge_mat(11, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(11, 2) = EDGE_MAT_ID("empty");
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
