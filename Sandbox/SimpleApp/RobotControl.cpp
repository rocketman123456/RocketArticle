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

#define ACTION_MAT_ID(action) action_2_mat[GlobalHashTable::HashString("StateMachine"_hash, action)]
#define NODE_MAT_ID(node) node_2_mat[GlobalHashTable::HashString("StateMachine"_hash, node)]
#define EDGE_MAT_ID(edge) edge_2_mat[GlobalHashTable::HashString("StateMachine"_hash, edge)]

void initialize_variable()
{
    transfer_node_mat = Eigen::Matrix<uint64_t, Eigen::Dynamic, Eigen::Dynamic>(18, 3);
    transfer_edge_mat = Eigen::Matrix<uint64_t, Eigen::Dynamic, Eigen::Dynamic>(18, 3);

    // Action id
    ACTION_MAT_ID("init_pos") = 0;
    ACTION_MAT_ID("rotation") = 1;
    ACTION_MAT_ID("walk") = 2;

    // Node id
    NODE_MAT_ID("init") = 0;
    NODE_MAT_ID("rot_00") = 1;
    NODE_MAT_ID("rot_01") = 2;
    NODE_MAT_ID("rot_02") = 3;
    NODE_MAT_ID("move_00") = 4;
    NODE_MAT_ID("move_01") = 5;
    NODE_MAT_ID("move_02") = 6;
    NODE_MAT_ID("move_03") = 7;
    NODE_MAT_ID("move_04") = 8;
    NODE_MAT_ID("move_05") = 9;
    NODE_MAT_ID("rot_rec_0") = 10;
    NODE_MAT_ID("rot_rec_1") = 11;
    NODE_MAT_ID("move_out_up") = 12;
    NODE_MAT_ID("move_out_mid") = 13;
    NODE_MAT_ID("move_out_down") = 14;
    NODE_MAT_ID("move_in_up") = 15;
    NODE_MAT_ID("move_in_mid") = 16;
    NODE_MAT_ID("move_in_down") = 17;

    // Edge id
    EDGE_MAT_ID("edge_01") = 0;
    EDGE_MAT_ID("edge_04") = 1;

    EDGE_MAT_ID("edge_10") = 2;
    EDGE_MAT_ID("edge_12") = 3;
    EDGE_MAT_ID("edge_23") = 4;
    EDGE_MAT_ID("edge_32") = 5;

    EDGE_MAT_ID("edge_45") = 6;

    EDGE_MAT_ID("edge_56") = 7;
    EDGE_MAT_ID("edge_67") = 8;
    EDGE_MAT_ID("edge_78") = 9;
    EDGE_MAT_ID("edge_89") = 10;
    EDGE_MAT_ID("edge_94") = 11;

    //EDGE_MAT_ID("edge_40") = 12;
    //EDGE_MAT_ID("edge_50") = 13;
    //EDGE_MAT_ID("edge_60") = 14;
    //EDGE_MAT_ID("edge_70") = 15;
    //EDGE_MAT_ID("edge_80") = 16;
    //EDGE_MAT_ID("edge_90") = 17;

    EDGE_MAT_ID("edge_3_10") = 18;
    EDGE_MAT_ID("edge_10_11") = 19;
    EDGE_MAT_ID("edge_11_1") = 20;

    EDGE_MAT_ID("edge_4_13") = 21;
    EDGE_MAT_ID("edge_5_13") = 22;
    EDGE_MAT_ID("edge_6_12") = 23;
    EDGE_MAT_ID("edge_7_16") = 24;
    EDGE_MAT_ID("edge_8_16") = 25;
    EDGE_MAT_ID("edge_9_15") = 26;

    EDGE_MAT_ID("edge_12_13") = 27;
    EDGE_MAT_ID("edge_13_14") = 28;
    EDGE_MAT_ID("edge_14_0") = 29;
    EDGE_MAT_ID("edge_15_16") = 30;
    EDGE_MAT_ID("edge_16_17") = 31;
    EDGE_MAT_ID("edge_17_0") = 32;

    EDGE_MAT_ID("empty") = UINT64_MAX;

    // init
    transfer_edge_mat(0, 0) = EDGE_MAT_ID("empty"); transfer_edge_mat(0, 1) = EDGE_MAT_ID("edge_01"); transfer_edge_mat(0, 2) = EDGE_MAT_ID("edge_04");
    // rot
    transfer_edge_mat(1, 0) = EDGE_MAT_ID("edge_10"); transfer_edge_mat(1, 1) = EDGE_MAT_ID("edge_12"); transfer_edge_mat(1, 2) = EDGE_MAT_ID("empty");
    transfer_edge_mat(2, 0) = EDGE_MAT_ID("empty"); transfer_edge_mat(2, 1) = EDGE_MAT_ID("edge_23"); transfer_edge_mat(2, 2) = EDGE_MAT_ID("empty");
    transfer_edge_mat(3, 0) = EDGE_MAT_ID("edge_3_10"); transfer_edge_mat(3, 1) = EDGE_MAT_ID("edge_32"); transfer_edge_mat(3, 2) = EDGE_MAT_ID("empty");
    // move
    transfer_edge_mat(4, 0) = EDGE_MAT_ID("edge_4_13"); transfer_edge_mat(4, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(4, 2) = EDGE_MAT_ID("edge_45");
    transfer_edge_mat(5, 0) = EDGE_MAT_ID("edge_5_13"); transfer_edge_mat(5, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(5, 2) = EDGE_MAT_ID("edge_56");
    transfer_edge_mat(6, 0) = EDGE_MAT_ID("edge_6_12"); transfer_edge_mat(6, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(6, 2) = EDGE_MAT_ID("edge_67");
    transfer_edge_mat(7, 0) = EDGE_MAT_ID("edge_7_16"); transfer_edge_mat(7, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(7, 2) = EDGE_MAT_ID("edge_78");
    transfer_edge_mat(8, 0) = EDGE_MAT_ID("edge_8_16"); transfer_edge_mat(8, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(8, 2) = EDGE_MAT_ID("edge_89");
    transfer_edge_mat(9, 0) = EDGE_MAT_ID("edge_9_15"); transfer_edge_mat(9, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(9, 2) = EDGE_MAT_ID("edge_94");
    //transfer_edge_mat(9, 0) = EDGE_MAT_ID("edge_9_15"); transfer_edge_mat(9, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(9, 2) = EDGE_MAT_ID("edge_94");
    // rot_rec
    transfer_edge_mat(10, 0) = EDGE_MAT_ID("edge_10_11"); transfer_edge_mat(10, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(10, 2) = EDGE_MAT_ID("empty");
    transfer_edge_mat(11, 0) = EDGE_MAT_ID("edge_11_1"); transfer_edge_mat(11, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(11, 2) = EDGE_MAT_ID("empty");
    // move_rec
    transfer_edge_mat(12, 0) = EDGE_MAT_ID("edge_12_13"); transfer_edge_mat(12, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(12, 2) = EDGE_MAT_ID("empty");
    transfer_edge_mat(13, 0) = EDGE_MAT_ID("edge_13_14"); transfer_edge_mat(13, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(13, 2) = EDGE_MAT_ID("empty");
    transfer_edge_mat(14, 0) = EDGE_MAT_ID("edge_14_0"); transfer_edge_mat(14, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(14, 2) = EDGE_MAT_ID("empty");
    transfer_edge_mat(15, 0) = EDGE_MAT_ID("edge_15_16"); transfer_edge_mat(15, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(15, 2) = EDGE_MAT_ID("empty");
    transfer_edge_mat(16, 0) = EDGE_MAT_ID("edge_16_17"); transfer_edge_mat(16, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(16, 2) = EDGE_MAT_ID("empty");
    transfer_edge_mat(17, 0) = EDGE_MAT_ID("edge_17_0"); transfer_edge_mat(17, 1) = EDGE_MAT_ID("empty"); transfer_edge_mat(17, 2) = EDGE_MAT_ID("empty");
}

uint64_t update_along_mat(const Vec<Variant>& data, const uint64_t state)
{
    uint64_t action = data[1].asStringId;
    uint64_t action_id = Get2Mat(action_2_mat, action);
    uint64_t state_id = Get2Mat(node_2_mat, state);
    uint64_t edge_id = transfer_edge_mat(state_id, action_id);
    uint64_t edge = Get2Map(edge_2_mat, edge_id);

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
