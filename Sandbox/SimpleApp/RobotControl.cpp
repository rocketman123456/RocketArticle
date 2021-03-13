#include "RobotControl.h"
#include "Utils/Hashing.h"

using namespace Rocket;

static void log_state_transfer(const uint64_t action, const uint64_t state)
{
    RK_CORE_TRACE("Current State {}, Get Action {}", 
        StateMachineHashTable::GetStringFromId(state),
        StateMachineHashTable::GetStringFromId(action)
    );
}

uint64_t init_2_other(const Vec<Variant>& data, const uint64_t state)
{
    uint64_t action = data[1].asStringId;
    log_state_transfer(action, state);

    if(action == StateMachineHashTable::HashString("rotation"))
        return StateMachineHashTable::HashString("egde_02");
    else if(action == StateMachineHashTable::HashString("walk"))
        return StateMachineHashTable::HashString("egde_03");
    return 0;
}

uint64_t rot_00_2_other(const Vec<Variant>& data, const uint64_t state)
{
    uint64_t action = data[1].asStringId;
    log_state_transfer(action, state);

    if(action == StateMachineHashTable::HashString("rotation"))
        return StateMachineHashTable::HashString("egde_21");
    else if(action == StateMachineHashTable::HashString("init_pos"))
        return StateMachineHashTable::HashString("egde_20");
    return 0;
}

uint64_t rot_01_2_other(const Vec<Variant>& data, const uint64_t state)
{
    uint64_t action = data[1].asStringId;
    log_state_transfer(action, state);

    if(action == StateMachineHashTable::HashString("rotation"))
        return StateMachineHashTable::HashString("egde_22");
    return 0;
}

uint64_t rot_02_2_other(const Vec<Variant>& data, const uint64_t state)
{
    uint64_t action = data[1].asStringId;
    log_state_transfer(action, state);

    if(action == StateMachineHashTable::HashString("rotation"))
        return StateMachineHashTable::HashString("egde_23");
    return 0;
}

uint64_t move_00_2_other(const Vec<Variant>& data, const uint64_t state)
{
    uint64_t action = data[1].asStringId;
    log_state_transfer(action, state);

    if(action == StateMachineHashTable::HashString("init_pos"))
        return StateMachineHashTable::HashString("egde_30");
    else if(action == StateMachineHashTable::HashString("walk"))
        return StateMachineHashTable::HashString("egde_31");
    return 0;
}

uint64_t move_01_2_other(const Vec<Variant>& data, const uint64_t state)
{
    uint64_t action = data[1].asStringId;
    log_state_transfer(action, state);

    if(action == StateMachineHashTable::HashString("walk"))
        return StateMachineHashTable::HashString("egde_32");
    return 0;
}

uint64_t move_02_2_other(const Vec<Variant>& data, const uint64_t state)
{
    uint64_t action = data[1].asStringId;
    log_state_transfer(action, state);

    if(action == StateMachineHashTable::HashString("walk"))
        return StateMachineHashTable::HashString("egde_33");
    return 0;
}

uint64_t move_03_2_other(const Vec<Variant>& data, const uint64_t state)
{
    uint64_t action = data[1].asStringId;
    log_state_transfer(action, state);

    if(action == StateMachineHashTable::HashString("walk"))
        return StateMachineHashTable::HashString("egde_34");
    return 0;
}

uint64_t move_04_2_other(const Vec<Variant>& data, const uint64_t state)
{
    uint64_t action = data[1].asStringId;
    log_state_transfer(action, state);

    if(action == StateMachineHashTable::HashString("walk"))
        return StateMachineHashTable::HashString("egde_35");
    return 0;
}

uint64_t move_05_2_other(const Vec<Variant>& data, const uint64_t state)
{
    uint64_t action = data[1].asStringId;
    log_state_transfer(action, state);

    if(action == StateMachineHashTable::HashString("walk"))
        return StateMachineHashTable::HashString("egde_36");
    return 0;
}
