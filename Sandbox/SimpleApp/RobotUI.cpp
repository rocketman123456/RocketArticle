#include "RobotUI.h"
#include "Module/EventManager.h"
#include "Module/GameLogic.h"
#include "Module/AssetLoader.h"

#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace Rocket;

float RobotUI::CalculateProgress(float start, float end, float current)
{
    float total = std::abs(end - start);
    float delta = std::abs(current - start);
    if(total < 1e-1)
        return 1;
    float progress = std::abs(delta / total);
    progress = std::clamp(progress, 0.0f, 1.0f);
    return progress;
}

void RobotUI::DrawRobotState()
{
    ImGui::Begin("Robot State", &showRobotState);
    
    for(int i = 0; i < 10; ++i)
    {
        uint64_t offset = std::max((int64_t)0, (int64_t)(motor_data[i].size() - max_motor_data_store));
        uint64_t data_size = std::min((int64_t)motor_data[i].size(), (int64_t)max_motor_data_store);
        char label[16] = {};
        sprintf(label, "Motor %2d", i+1);
        char overlay[32];
        sprintf(overlay, "size %d offset %d", (uint32_t)motor_data[i].size(), (uint32_t)offset);
        float result = CalculateProgress(motor_data_start[i], motor_data_target[i], motor_data_curr[i]);
        ImGui::ProgressBar(result, ImVec2(0.0f, 0.0f)); ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); ImGui::Text("Motor Progress %2d", i+1);
        ImGui::PlotLines(label, motor_data[i].data() + offset, data_size, 0, overlay, -100.0f, 100.0f, ImVec2(0, 80.0f));
    }

    for(int i = 0; i < 3; ++i)
    {
        uint64_t offset = std::max((int64_t)0, (int64_t)(imu_data[i].size() - max_imu_data_store));
        uint64_t data_size = std::min((int64_t)imu_data[i].size(), (int64_t)max_imu_data_store);
        char label[16] = {};
        sprintf(label, "IMU %d", i);
        char overlay[32];
        sprintf(overlay, "size %d offset %d", (uint32_t)imu_data[i].size(), (uint32_t)offset);
        ImGui::PlotLines(label, imu_data[i].data() + offset, (int32_t)data_size, 0, overlay, -90.0f, 90.0f, ImVec2(0, 80.0f));
    }

    ImGui::End();
}

void RobotUI::DrawRobotSetting()
{
    ImGui::Begin("Rocket Engine");

    ImGui::Separator();
    init = ImGui::Button("Init");
    walk = ImGui::Button("Walk"); 
    rotation = ImGui::Button("Rotation");

    ImGui::Separator();
    if(node_curr)
        ImGui::TextColored(ImVec4({1.0, 0.0, 0.0, 1.0}), "Current State : %s", node_curr->name.c_str());

    ImGui::Separator();
    const char* modes[] = { "mode 01", "mode 02"};
    const char* motors[] = { "motor 01", "motor 02", "motor 03", "motor 04", "motor 05", "motor 06", "motor 07", "motor 08", "motor 09", "motor 10"};
    ImGui::Combo("select rotation mode", &rotate_mode, modes, IM_ARRAYSIZE(modes));
    ImGui::Combo("select motor", &motor_id, motors, IM_ARRAYSIZE(motors));

    ImGui::Separator();
    ImGui::InputInt4("Led Data", led_data);
    ImGui::InputFloat("Motor", &motor_data_target[motor_id], 0.001);
    ImGui::InputInt("Motor PWM", &motor_pwm);
    ImGui::InputInt("Valve Control", &valve_data);

    led_set = ImGui::Button("Set Led");
    set_motor_data = ImGui::Button("Set Motor Pos");
    get_motor_data = ImGui::Button("Get Motor Pos");
    set_motor_pwm = ImGui::Button("Set PWM");
    reset_motor = ImGui::Button("Reset Motor");
    set_valve = ImGui::Button("Set Valve");
    
    SingleControl();

    ImGui::Separator();
    ImGui::Checkbox("Show Robot State", &showRobotState);

    ImGui::Separator();
    ImGui::InputText("Save Data Name", file_name, 1024);
    save_data = ImGui::Button("Save Data");
    if(save_data)
    {
        RK_CORE_TRACE("{}", file_name);
        SaveData();
    }
    clear_data = ImGui::Button("Clear Data");
    if(clear_data)
    {
        for(int j = 0; j < 10; j++)
            motor_data[j].clear();
        for(int k = 0; k < 3; k++)
            imu_data[k].clear();
    }

    ImGui::Separator();
    ImGui::Text("Robot Data");
    ImGui::InputDouble("W(mm)", &W, 0.01);
    ImGui::InputDouble("L(mm)", &L, 0.01);
    ImGui::InputDouble("h(mm)", &h, 0.01);
    ImGui::InputDouble("stride(mm)", &stride, 0.01);
    ImGui::InputDouble("up_height(mm)", &up_height, 0.01);
    ImGui::InputDouble("angle_x(°)", &angle_x_curr, 0.001);
    ImGui::InputDouble("angle_y(°)", &angle_y_curr, 0.001);
    ImGui::InputInt("direction", &direction, 1);

    ImGui::End();
}

void RobotUI::SingleControl()
{
    EventVarVec var;
    var.resize(4);
    var[0].type = Variant::TYPE_STRING_ID;
    var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "ui_event_send_data");

    if(led_set)
    {
        var[1].type = Variant::TYPE_UINT32;
        var[1].asUInt32 = 0x01;

        var[2].type = Variant::TYPE_UINT32;
        var[2].asUInt32 = 0x01;

        uint8_t led_data_u8[4];
        for(int i = 0; i < 4; ++i)
            led_data_u8[i] = (uint8_t)led_data[i];
        var[3].type = Variant::TYPE_UINT32;
        memcpy(&var[3].asUInt32, led_data_u8, sizeof(led_data_u8));
    }
    else if(set_motor_pwm)
    {
        var[1].type = Variant::TYPE_UINT32;
        var[1].asUInt32 = motor_id + 1;

        var[2].type = Variant::TYPE_UINT32;
        var[2].asUInt32 = 0x02;

        var[3].type = Variant::TYPE_FLOAT;
        var[3].asInt32 = motor_pwm;
    }
    else if(get_motor_data)
    {
        var[1].type = Variant::TYPE_UINT32;
        var[1].asUInt32 = motor_id + 1;

        var[2].type = Variant::TYPE_UINT32;
        var[2].asUInt32 = 0x05;
    }
    else if(reset_motor)
    {
        var[1].type = Variant::TYPE_UINT32;
        var[1].asUInt32 = motor_id + 1;

        var[2].type = Variant::TYPE_UINT32;
        var[2].asUInt32 = 0x03;

        //motor_data_curr[motor_id] = 0;//motor_data_curr[motor_id] - motor_data_target[motor_id];
        motor_data_target[motor_id] = 0;

        var[3].type = Variant::TYPE_FLOAT;
        var[3].asFloat = motor_data_target[motor_id];
    }
    else if(set_motor_data)
    {
        var[1].type = Variant::TYPE_UINT32;
        var[1].asUInt32 = motor_id + 1;

        var[2].type = Variant::TYPE_UINT32;
        var[2].asUInt32 = 0x04;

        var[3].type = Variant::TYPE_FLOAT;
        var[3].asFloat = motor_data_target[motor_id];

        motor_data_start[motor_id] = motor_data_curr[motor_id];
    }
    else if(set_valve)
    {
        var[1].type = Variant::TYPE_UINT32;
        var[1].asUInt32 = 0x00;

        var[2].type = Variant::TYPE_UINT32;
        var[2].asUInt32 = 0x06;

        var[3].type = Variant::TYPE_INT32;
        var[3].asInt32 = valve_data;
    }

    if(set_motor_pwm || set_motor_data || set_valve || get_motor_data || reset_motor || led_set)
    {
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
}

void RobotUI::SaveData()
{
    uint32_t data_size = motor_data[0].size();
    std::stringstream fout;
    for(int i = 0; i < data_size; ++i)
    {
        for(int j = 0; j < 10; j++)
        {
            if(i < motor_data[j].size())
                fout << motor_data[j][i];
            else
                fout << "0.0";
            fout << ",";
        }
        for(int k = 0; k < 3; k++)
        {
            if(i < imu_data[k].size())
                fout << imu_data[k][i];
            else
                fout << "0.0";
            fout << ",";
        }
        fout << std::endl;
    }

    String str = fout.str();
    g_AssetLoader->SyncOpenAndWriteStringToTextFile(file_name, str);
}

void RobotUI::Draw()
{
    DrawRobotSetting();

    if(showRobotState)
    {
        DrawRobotState();
    }

    node_curr = g_GameLogic->GetStateMachine()->GetCurrentNode();

    Calculation();

    EventVarVec var;
    var.resize(2 + 10 + 1);
    var[0].type = Variant::TYPE_STRING_ID;
    var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "ui_event_response_data");
    var[1].type = Variant::TYPE_UINT32;
    var[1].asUInt32 = 0x01; // cmd
    for(int i = 0; i < 10; ++i)
    {
        var[2 + i].type = Variant::TYPE_FLOAT;
        var[2 + i].asFloat = motor_data_curr[i]; // data
    }
    EventPtr event = CreateRef<Event>(var);
    g_EventManager->QueueEvent(event);
}

bool RobotUI::OnResponseEvent(EventPtr& e)
{
    // get motor data
    if(e->variable[2].asUInt32 == 0x09)
    {
        uint32_t id = e->variable[1].asUInt32 - 0x01;
        motor_data_curr[id] = e->variable[3].asFloat;
        motor_data[id].push_back(motor_data_curr[id]);
    }
    // imu data
    else if(e->variable[2].asUInt32 == 0x08)
    {
        uint32_t id = e->variable[1].asUInt32 - 0x11;
        imu_data_curr[id] = e->variable[3].asFloat;
        imu_data[id].push_back(imu_data_curr[id]);
    }
    
    return false;
}

RobotUI::RobotUI()
{
    node_calculate_stage[GlobalHashTable::HashString("StateMachine"_hash, "init")] = 0;
    node_calculate_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_00")] = 1;
    node_calculate_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_01")] = 2;
    node_calculate_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_02")] = 3;
    node_calculate_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_03")] = 4;
    node_calculate_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_04")] = 5;
    node_calculate_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_05")] = 6;
    node_calculate_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_06")] = 1;
    
    node_recovery_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_01")] = 1;
    node_recovery_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_02")] = 0;
    node_recovery_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_03")] = 4;
    node_recovery_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_04")] = 4;
    node_recovery_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_05")] = 3;
    node_recovery_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_06")] = 1;

    node_recovery_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_out_up")] = 1;
    node_recovery_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_out_mid")] = 2;
    node_recovery_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_out_down")] = 6;
    node_recovery_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_in_up")] = 4;
    node_recovery_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_in_mid")] = 5;
    node_recovery_stage[GlobalHashTable::HashString("StateMachine"_hash, "move_in_down")] = 6;
}

void RobotUI::Calculation()
{
    if(!(init || walk || rotation))
        return;
    
    EventVarVec var;
    var.resize(2 + 10 + 1);
    // Event Type
    var[0].type = Variant::TYPE_STRING_ID;
    var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "ui_event_logic");
    // Action
    var[1].type = Variant::TYPE_STRING_ID;
    var[1].asStringId = 0;

    if(init) { var[1].asStringId = GlobalHashTable::HashString("StateMachine"_hash, "initialize"); }
    if(walk) { var[1].asStringId = GlobalHashTable::HashString("StateMachine"_hash, "movement"); }
    if(rotation) { var[1].asStringId = GlobalHashTable::HashString("StateMachine"_hash, "rotation"); }

    for(int i = 0; i < 10; ++i)
    {
        motor_data_start[i] = motor_data_curr[i];
    }

    //--------------------------------------------------------------//
    //--------------------------------------------------------------//
    //--------------------------------------------------------------//
    if(
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "init") && rotation) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "rot_00") && init) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "rot_rec_02") && init))
    {
        // inner leg down
        motor_data_target[0] = 0;
        motor_data_target[1] = rotation ? -up_height : 0;
        motor_data_target[2] = rotation ? -up_height : 0;
        motor_data_target[3] = 0;
        motor_data_target[4] = 0;
        motor_data_target[5] = rotation ? -up_height : 0;
        motor_data_target[6] = rotation ? -up_height : 0;
        motor_data_target[7] = 0;
        motor_data_target[8] = 0;
        motor_data_target[9] = 0;

        SetEventData(var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    else if(
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "rot_00") && rotation) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "rot_02") && rotation) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "rot_02") && init))
    {
        if(init)
        {
            angle_y_curr = angle_x_curr = 0;
        }

        if(rotate_mode == 0)
        {
            CalculateRotation(angle_x_prev, angle_y_curr);
            SetEventData(var);
            EventPtr event = CreateRef<Event>(var);
            g_EventManager->QueueEvent(event);

            CalculateRotation(angle_x_curr, angle_y_curr);
            SetEventData(var);
            event = CreateRef<Event>(var);
            g_EventManager->QueueEvent(event);
        }
        else if(rotate_mode == 1)
        {
            CalculateRotation(angle_x_curr, angle_y_prev);
            SetEventData(var);
            EventPtr event = CreateRef<Event>(var);
            g_EventManager->QueueEvent(event);

            CalculateRotation(angle_x_curr, angle_y_curr);
            SetEventData(var);
            event = CreateRef<Event>(var);
            g_EventManager->QueueEvent(event);
        }
        
        angle_x_prev = angle_x_curr;
        angle_y_prev = angle_y_curr;
    }
    //--------------------------------------------------------------//
    //--------------------------------------------------------------//
    //--------------------------------------------------------------//
    else if(
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "init") && walk) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_00") && walk) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_01") && walk) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_02") && walk) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_03") && walk) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_04") && walk) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_05") && walk) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_06") && walk))
    {
        CalculateMovement(node_calculate_stage.at(node_curr->id));
        SetEventData(var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    //--------------------------------------------------------------//
    //--------------------------------------------------------------//
    //--------------------------------------------------------------//
    else if(
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_01") && init) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_02") && init) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_03") && init) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_04") && init) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_05") && init) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_06") && init) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_out_up") && init) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_out_mid") && init) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_out_down") && init) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_in_up") && init) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_in_mid") && init) ||
        (node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_in_down") && init))
    {
        CalculateMovementRecover(node_recovery_stage.at(node_curr->id));
        SetEventData(var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
}

void RobotUI::SetEventData(Rocket::EventVarVec& var)
{
    // set motor data
    for(int i = 0; i < 10 ; ++i)
    {
        motor_data_start[i] = motor_data_curr[i];
        var[2 + i].type = Variant::TYPE_FLOAT;
        var[2 + i].asFloat = motor_data_target[i];
    }
    // set valve data
    var[12].type = Variant::TYPE_UINT32;
    var[12].asUInt32 = valve_data_set;
}

void RobotUI::CalculateRotation(double x, double y)
{
    double L0, L1, L2, L3;
    L0 = h;
    double tx = x / 180.0 * M_PI;
    double ty = y / 180.0 * M_PI;
    if(rotate_mode == 0)
    {
        L1 = abs(cos(tx)*(L*sin(ty) + h*cos(ty)))/(abs(cos(tx))*abs(cos(ty)));
        L2 = abs(L*cos(tx)*sin(ty) - W*sin(tx) + h*cos(tx)*cos(ty))/(abs(cos(tx))*abs(cos(ty)));
        L3 = abs(W*sin(tx) - h*cos(tx)*cos(ty))/(abs(cos(tx))*abs(cos(ty)));

        valve_data_target[0] = 1;   // 2 x
        valve_data_target[1] = 1;   // 2 y
        valve_data_target[2] = 1;   // 1 x
        valve_data_target[3] = 1;   // 1 y
        valve_data_target[4] = 0;   // 0 x
        valve_data_target[5] = 1;   // 0 y
        valve_data_target[6] = 0;   // 3 x
        valve_data_target[7] = 0;   // 3 y
        valve_data_target[8] = 0;   // vacuum inner
        valve_data_target[9] = 1;   // vacuum outter
        valve_data_target[10] = 0;   // vacuum inner
        valve_data_target[11] = 0;   // vacuum outter
    }
    else if(rotate_mode == 1)
    {
        L1 = abs(L*sin(ty) + h*cos(tx)*cos(ty))/(abs(cos(tx))*abs(cos(ty)));
        L2 = sqrt(abs(L*sin(ty) - W*cos(ty)*sin(tx) + h*cos(tx)*cos(ty))*(sin(tx)*sin(tx)*sin(ty)*sin(ty) + 1))/(abs(cos(tx))*abs(cos(ty)));
        L3 = sqrt(cos(ty)*cos(ty)*(W*sin(tx) - h*cos(tx))*(W*sin(tx) - h*cos(tx)) + (sin(ty)*sin(ty)*(W*cos(tx) + h*sin(tx))*(W*cos(tx) + h*sin(tx)))/(cos(ty)*cos(ty)) + (sin(tx)*sin(tx)*(W*cos(tx) + h*sin(tx))*(W*cos(tx) + h*sin(tx)))/(cos(tx)*cos(tx)*cos(ty)*cos(ty)));
    
        valve_data_target[0] = 1;   // 2 x
        valve_data_target[1] = 1;   // 2 y
        valve_data_target[2] = 1;   // 1 x
        valve_data_target[3] = 1;   // 1 y
        valve_data_target[4] = 0;   // 0 x
        valve_data_target[5] = 1;   // 0 y
        valve_data_target[6] = 0;   // 3 x
        valve_data_target[7] = 0;   // 3 y
        valve_data_target[8] = 0;   // vacuum inner
        valve_data_target[9] = 1;   // vacuum outter
        valve_data_target[10] = 0;   // vacuum inner
        valve_data_target[11] = 0;   // vacuum outter
    }

    motor_data_target[0] = L2 - h;
    motor_data_target[1] = -up_height;  // inner
    motor_data_target[2] = -up_height;  // inner
    motor_data_target[3] = L3 - h;
    motor_data_target[4] = 0;
    motor_data_target[5] = -up_height;  // inner
    motor_data_target[6] = -up_height;  // inner
    motor_data_target[7] = L1 - h;
    motor_data_target[8] = 0;
    motor_data_target[9] = 0;

    valve_data_set = 0;
    for(int i = 0; i < 12; ++i)
        valve_data_set = valve_data_set | (valve_data_target[i] << i);
}

void RobotUI::CalculateMovement(int32_t stage)
{
    switch(stage)
    {
    case 0:
        motor_data_target[0] = -up_height;
        motor_data_target[1] = 0;
        motor_data_target[2] = 0;
        motor_data_target[3] = -up_height;
        motor_data_target[4] = -up_height;
        motor_data_target[5] = 0;
        motor_data_target[6] = 0;
        motor_data_target[7] = -up_height;
        motor_data_target[8] = 0;
        motor_data_target[9] = 0;
        valve_data_target[8] = 1;   // vacuum inner
        valve_data_target[9] = 0;   // vacuum outter
        valve_data_target[10] = 0;   // vacuum inner
        valve_data_target[11] = 1;   // vacuum outter
        break;
    case 1:
        motor_data_target[0] = -up_height;
        motor_data_target[1] = 0;
        motor_data_target[2] = 0;
        motor_data_target[3] = -up_height;
        motor_data_target[4] = -up_height;
        motor_data_target[5] = 0;
        motor_data_target[6] = 0;
        motor_data_target[7] = -up_height;
        motor_data_target[8] = stride;
        motor_data_target[9] = stride;
        valve_data_target[8] = 1;   // vacuum inner
        valve_data_target[9] = 0;   // vacuum outter
        valve_data_target[10] = 0;   // vacuum inner
        valve_data_target[11] = 0;   // vacuum outter
        break;
    case 2:
        motor_data_target[0] = 0;
        motor_data_target[1] = 0;
        motor_data_target[2] = 0;
        motor_data_target[3] = 0;
        motor_data_target[4] = 0;
        motor_data_target[5] = 0;
        motor_data_target[6] = 0;
        motor_data_target[7] = 0;
        motor_data_target[8] = stride;
        motor_data_target[9] = stride;
        valve_data_target[8] = 1;   // vacuum inner
        valve_data_target[9] = 1;   // vacuum outter
        valve_data_target[10] = 0;   // vacuum inner
        valve_data_target[11] = 0;   // vacuum outter
        break;
    case 3:
        motor_data_target[0] = 0;
        motor_data_target[1] = -up_height;
        motor_data_target[2] = -up_height;
        motor_data_target[3] = 0;
        motor_data_target[4] = 0;
        motor_data_target[5] = -up_height;
        motor_data_target[6] = -up_height;
        motor_data_target[7] = 0;
        motor_data_target[8] = stride;
        motor_data_target[9] = stride;
        valve_data_target[8] = 0;   // vacuum inner
        valve_data_target[9] = 1;   // vacuum outter
        valve_data_target[10] = 1;   // vacuum inner
        valve_data_target[11] = 0;   // vacuum outter
        break;
    case 4:
        motor_data_target[0] = 0;
        motor_data_target[1] = -up_height;
        motor_data_target[2] = -up_height;
        motor_data_target[3] = 0;
        motor_data_target[4] = 0;
        motor_data_target[5] = -up_height;
        motor_data_target[6] = -up_height;
        motor_data_target[7] = 0;
        motor_data_target[8] = -stride;
        motor_data_target[9] = -stride;
        valve_data_target[8] = 0;   // vacuum inner
        valve_data_target[9] = 1;   // vacuum outter
        valve_data_target[10] = 0;   // vacuum inner
        valve_data_target[11] = 0;   // vacuum outter
        break;
    case 5:
        motor_data_target[0] = 0;
        motor_data_target[1] = 0;
        motor_data_target[2] = 0;
        motor_data_target[3] = 0;
        motor_data_target[4] = 0;
        motor_data_target[5] = 0;
        motor_data_target[6] = 0;
        motor_data_target[7] = 0;
        motor_data_target[8] = -stride;
        motor_data_target[9] = -stride;
        valve_data_target[8] = 1;   // vacuum inner
        valve_data_target[9] = 1;   // vacuum outter
        valve_data_target[10] = 0;   // vacuum inner
        valve_data_target[11] = 0;   // vacuum outter
        break;
    case 6:
        motor_data_target[0] = -up_height;
        motor_data_target[1] = 0;
        motor_data_target[2] = 0;
        motor_data_target[3] = -up_height;
        motor_data_target[4] = -up_height;
        motor_data_target[5] = 0;
        motor_data_target[6] = 0;
        motor_data_target[7] = -up_height;
        motor_data_target[8] = -stride;
        motor_data_target[9] = -stride;
        valve_data_target[8] = 1;   // vacuum inner
        valve_data_target[9] = 0;   // vacuum outter
        valve_data_target[10] = 0;   // vacuum inner
        valve_data_target[11] = 1;   // vacuum outter
        break;
    default:
        break;
    }

    float sign = (direction > 0) - (direction < 0);
    motor_data_target[8] *= sign;
    motor_data_target[9] *= sign;
    
    for(int i = 0; i < 8; ++i)
        valve_data_target[i] = 0;

    valve_data_set = 0;
    for(int i = 0; i < 12; ++i)
        valve_data_set = valve_data_set | (valve_data_target[i] << i);
}

void RobotUI::CalculateMovementRecover(int32_t stage)
{
    switch(stage)
    {
    case 0:
        motor_data_target[0] = -up_height;
        motor_data_target[1] = 0;
        motor_data_target[2] = 0;
        motor_data_target[3] = -up_height;
        motor_data_target[4] = -up_height;
        motor_data_target[5] = 0;
        motor_data_target[6] = 0;
        motor_data_target[7] = -up_height;
        //motor_data_target[8] = 0;
        //motor_data_target[9] = 0;
        valve_data_target[8] = 1;   // vacuum inner
        valve_data_target[9] = 0;   // vacuum outter
        valve_data_target[10] = 0;   // vacuum inner
        valve_data_target[11] = 1;   // vacuum outter
        break;
    case 1:
        motor_data_target[0] = -up_height;
        motor_data_target[1] = 0;
        motor_data_target[2] = 0;
        motor_data_target[3] = -up_height;
        motor_data_target[4] = -up_height;
        motor_data_target[5] = 0;
        motor_data_target[6] = 0;
        motor_data_target[7] = -up_height;
        motor_data_target[8] = 0;
        motor_data_target[9] = 0;
        valve_data_target[8] = 1;   // vacuum inner
        valve_data_target[9] = 0;   // vacuum outter
        valve_data_target[10] = 0;   // vacuum inner
        valve_data_target[11] = 0;   // vacuum outter
        break;
    case 3:
        motor_data_target[0] = 0;
        motor_data_target[1] = -up_height;
        motor_data_target[2] = -up_height;
        motor_data_target[3] = 0;
        motor_data_target[4] = 0;
        motor_data_target[5] = -up_height;
        motor_data_target[6] = -up_height;
        motor_data_target[7] = 0;
        //motor_data_target[8] = 0;
        //motor_data_target[9] = 0;
        valve_data_target[8] = 0;   // vacuum inner
        valve_data_target[9] = 1;   // vacuum outter
        valve_data_target[10] = 1;   // vacuum inner
        valve_data_target[11] = 0;   // vacuum outter
        break;
    case 4:
        motor_data_target[0] = 0;
        motor_data_target[1] = -up_height;
        motor_data_target[2] = -up_height;
        motor_data_target[3] = 0;
        motor_data_target[4] = 0;
        motor_data_target[5] = -up_height;
        motor_data_target[6] = -up_height;
        motor_data_target[7] = 0;
        motor_data_target[8] = 0;
        motor_data_target[9] = 0;
        valve_data_target[8] = 0;   // vacuum inner
        valve_data_target[9] = 1;   // vacuum outter
        valve_data_target[10] = 0;   // vacuum inner
        valve_data_target[11] = 0;   // vacuum outter
        break;
    case 2: [[fallthrough]];
    case 5: [[fallthrough]];
    case 6:
        motor_data_target[0] = 0;
        motor_data_target[1] = 0;
        motor_data_target[2] = 0;
        motor_data_target[3] = 0;
        motor_data_target[4] = 0;
        motor_data_target[5] = 0;
        motor_data_target[6] = 0;
        motor_data_target[7] = 0;
        motor_data_target[8] = 0;
        motor_data_target[9] = 0;
        valve_data_target[8] = 1;   // vacuum inner
        valve_data_target[9] = 1;   // vacuum outter
        valve_data_target[10] = 0;   // vacuum inner
        valve_data_target[11] = 0;   // vacuum outter
        break;
    default:
        break;
    }
    
    for(int i = 0; i < 8; ++i)
        valve_data_target[i] = 0;
    
    valve_data_set = 0;
    for(int i = 0; i < 12; ++i)
        valve_data_set = valve_data_set | (valve_data_target[i] << i);
}
