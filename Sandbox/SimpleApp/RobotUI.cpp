#include "RobotUI.h"
#include "Module/EventManager.h"
#include "Module/GameLogic.h"

#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace Rocket;

void RobotUI::DrawRobotState()
{
    ImGui::Begin("Robot State", &showRobotState);
    static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };

    static float values[90] = {};
    static int values_offset = 0;
    static double refresh_time = 0.0;
    if (refresh_time == 0.0)
        refresh_time = ImGui::GetTime();
    while (refresh_time < ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
    {
        static float phase = 0.0f;
        values[values_offset] = cosf(phase);
        values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
        phase += 0.10f * values_offset;
        refresh_time += 1.0f / 60.0f;
    }

    float average = 0.0f;
    for (int n = 0; n < IM_ARRAYSIZE(values); n++)
        average += values[n];
    average /= (float)IM_ARRAYSIZE(values);
    char overlay[32];
    sprintf(overlay, "avg %f", average);

    static float progress = 0.0f, progress_dir = 1.0f;
    progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
    if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
    if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }

    for(int i = 0; i < 10; ++i)
    {
        uint64_t offset = std::max((int64_t)0, (int64_t)(motor_data[i].size() - max_motor_data_store));
        char label[16] = {};
        sprintf(label, "Motor %2d", i);
        float result = CalculateProgress(motor_data_start[i], motor_data_target[i], motor_data_curr[i]);
        ImGui::ProgressBar(result, ImVec2(0.0f, 0.0f)); ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); ImGui::Text("Motor Progress %2d", i);
        ImGui::PlotLines(label, motor_data[i].data(), motor_data[i].size(), offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));
    }

    ImGui::End();
}

void RobotUI::DrawRobotSetting()
{
    ImGui::Begin("Rocket", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Rocket Vulkan Render");

    ImGui::Separator();
    init = ImGui::Button("Init");
    walk = ImGui::Button("Walk"); 
    rotation = ImGui::Button("Rotation");

    ImGui::Separator();
    const char* modes[] = { "mode 01", "mode 02"};
    ImGui::Combo("select rotation mode", &rotate_mode, modes, IM_ARRAYSIZE(modes));
    const char* motors[] = { "motor 01", "motor 02", "motor 03", "motor 04", "motor 05", "motor 06", "motor 07", "motor 08", "motor 09", "motor 10"};
    ImGui::Combo("select motor", &motor_id, motors, IM_ARRAYSIZE(motors));
    get_motor_data = ImGui::Button("Get Data");
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::InputFloat("Motor", &motor_data_target[motor_id], 0.001);
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    set_motor_data = ImGui::Button("Set Data");
    reset_motor = ImGui::Button("Reset Motor");

    ImGui::Separator();
    ImGui::Checkbox("Show Robot State", &showRobotState);

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

void RobotUI::Draw()
{
    DrawRobotSetting();

    if(showRobotState)
    {
        DrawRobotState();
    }

    node_curr = g_GameLogic->GetStateMachine()->GetCurrentNode();

    //ImGui::ShowDemoWindow();

    Calculation();
}

bool RobotUI::OnResponseEvent(EventPtr& e)
{
    for(int i = 0; i < 10; ++i)
    {
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        motor_data[i].push_back(r);
        //motor_data[i].push(e->Var[2 + i].asFloat);
        //if(motor_data[i].size() > max_motor_data_store)
        //{
        //    motor_data[i];
        //}
    }
    return false;
}

void RobotUI::Calculation()
{
    EventVarVec var;
    var.resize(2 + 10 + 10);
    // Event Type
    var[0].type = Variant::TYPE_STRING_ID;
    var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "ui_event_logic");
    // Action
    var[1].type = Variant::TYPE_STRING_ID;
    var[1].asStringId = 0;

    if(init) { var[1].asStringId = GlobalHashTable::HashString("StateMachine"_hash, "initialize"); }
    if(walk) { var[1].asStringId = GlobalHashTable::HashString("StateMachine"_hash, "movement"); }
    if(rotation) { var[1].asStringId = GlobalHashTable::HashString("StateMachine"_hash, "rotation"); }

    if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "init") && rotation)
    {
        // Move Inner Up

        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "rot_00") && rotation)
    {
        if(rotate_mode == 0)
        {
            CalculateRotation(angle_x_prev, angle_y_curr, var);
            EventPtr event = CreateRef<Event>(var);
            g_EventManager->QueueEvent(event);

            CalculateRotation(angle_x_curr, angle_y_curr, var);
            event = CreateRef<Event>(var);
            g_EventManager->QueueEvent(event);
        }
        else if(rotate_mode == 1)
        {
            CalculateRotation(angle_x_curr, angle_y_prev, var);
            EventPtr event = CreateRef<Event>(var);
            g_EventManager->QueueEvent(event);

            CalculateRotation(angle_x_curr, angle_y_curr, var);
            event = CreateRef<Event>(var);
            g_EventManager->QueueEvent(event);
        }
        
        angle_x_prev = angle_x_curr;
        angle_y_prev = angle_y_curr;
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "rot_02") && rotation)
    {
        if(rotate_mode == 0)
        {
            CalculateRotation(angle_x_prev, angle_y_curr, var);
            EventPtr event = CreateRef<Event>(var);
            g_EventManager->QueueEvent(event);

            CalculateRotation(angle_x_curr, angle_y_curr, var);
            event = CreateRef<Event>(var);
            g_EventManager->QueueEvent(event);
        }
        else if(rotate_mode == 1)
        {
            CalculateRotation(angle_x_curr, angle_y_prev, var);
            EventPtr event = CreateRef<Event>(var);
            g_EventManager->QueueEvent(event);

            CalculateRotation(angle_x_curr, angle_y_curr, var);
            event = CreateRef<Event>(var);
            g_EventManager->QueueEvent(event);
        }
        
        angle_x_prev = angle_x_curr;
        angle_y_prev = angle_y_curr;
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "rot_02") && init)
    {
        if(rotate_mode == 0)
        {
            CalculateRotation(angle_x_prev, 0, var);
            EventPtr event = CreateRef<Event>(var);
            g_EventManager->QueueEvent(event);

            CalculateRotation(0, 0, var);
            event = CreateRef<Event>(var);
            g_EventManager->QueueEvent(event);
        }
        else if(rotate_mode == 1)
        {
            CalculateRotation(0, angle_y_prev, var);
            EventPtr event = CreateRef<Event>(var);
            g_EventManager->QueueEvent(event);

            CalculateRotation(0, 0, var);
            event = CreateRef<Event>(var);
            g_EventManager->QueueEvent(event);
        }
        
        angle_x_prev = angle_x_curr;
        angle_y_prev = angle_y_curr;
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "rot_rec_02") && init)
    {
        // inner leg down
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }

    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "init") && walk)
    {
        CalculateMovement(0, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_00") && walk)
    {
        CalculateMovement(1, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_01") && walk)
    {
        CalculateMovement(2, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_02") && walk)
    {
        CalculateMovement(3, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_03") && walk)
    {
        CalculateMovement(4, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_04") && walk)
    {
        CalculateMovement(5, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_05") && walk)
    {
        CalculateMovement(6, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_06") && walk)
    {
        CalculateMovement(1, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }

    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_01") && init)
    {
        CalculateMovementRecover(0, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_02") && init)
    {
        CalculateMovementRecover(0, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_03") && init)
    {
        CalculateMovementRecover(0, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_04") && init)
    {
        CalculateMovementRecover(0, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_05") && init)
    {
        CalculateMovementRecover(0, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_06") && init)
    {
        CalculateMovementRecover(0, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }

    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_out_up") && init)
    {
        CalculateMovementRecover(0, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_out_mid") && init)
    {
        CalculateMovementRecover(0, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_out_down") && init)
    {
        CalculateMovementRecover(0, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }

    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_in_up") && init)
    {
        CalculateMovementRecover(0, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_in_mid") && init)
    {
        CalculateMovementRecover(0, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
    else if(node_curr->id == GlobalHashTable::HashString("StateMachine"_hash, "move_in_down") && init)
    {
        CalculateMovementRecover(0, var);
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
}

float RobotUI::CalculateProgress(float start, float end, float current)
{
    float total = end - start;
    float delta = current - start;
    if(total < 1e-4)
    {
        return 1;
    }
    float progress = delta / total;
    return progress;
}

void RobotUI::CalculateRotation(double x, double y, EventVarVec& var)
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
    }
    else if(rotate_mode == 1)
    {
        L1 = abs(L*sin(ty) + h*cos(tx)*cos(ty))/(abs(cos(tx))*abs(cos(ty)));
        L2 = sqrt(abs(L*sin(ty) - W*cos(ty)*sin(tx) + h*cos(tx)*cos(ty))*(sin(tx)*sin(tx)*sin(ty)*sin(ty) + 1))/(abs(cos(tx))*abs(cos(ty)));
        L3 = sqrt(cos(ty)*cos(ty)*(W*sin(tx) - h*cos(tx))*(W*sin(tx) - h*cos(tx)) + (sin(ty)*sin(ty)*(W*cos(tx) + h*sin(tx))*(W*cos(tx) + h*sin(tx)))/(cos(ty)*cos(ty)) + (sin(tx)*sin(tx)*(W*cos(tx) + h*sin(tx))*(W*cos(tx) + h*sin(tx)))/(cos(tx)*cos(tx)*cos(ty)*cos(ty)));
    }

    // set motor data
    for(int i = 0; i < 10 ; ++i)
    {
        var[2 + i].type = Variant::TYPE_FLOAT;
        var[2 + i].asFloat = motor_data_target[i];
    }
    // set valve data
    for(int i = 0; i < 10 ; ++i)
    {
        var[12 + i].type = Variant::TYPE_INT32;
        var[12 + i].asFloat = valve_data_target[i];
    }
}

void RobotUI::CalculateMovement(int32_t stage, EventVarVec& var)
{
    double motor_pos[10] = {0};

    switch(stage)
    {
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        break;
    default:
        break;
    }

    if(direction > 0)
    {

    }
    else if(direction < 0)
    {

    }
    else
    {

    }

    // set motor data
    for(int i = 0; i < 10 ; ++i)
    {
        var[2 + i].type = Variant::TYPE_FLOAT;
        var[2 + i].asFloat = motor_data_target[i];
    }
    // set valve data
    for(int i = 0; i < 10 ; ++i)
    {
        var[12 + i].type = Variant::TYPE_INT32;
        var[12 + i].asFloat = valve_data_target[i];
    }
}

void RobotUI::CalculateMovementRecover(int32_t stage, EventVarVec& var)
{

}
