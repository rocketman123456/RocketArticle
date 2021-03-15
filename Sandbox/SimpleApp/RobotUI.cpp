#include "RobotUI.h"
#include "Module/EventManager.h"

#include <algorithm>

using namespace Rocket;

void RobotUI::CalculateRobot()
{
}

void RobotUI::Draw()
{
    ImGui::Begin("Rocket", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Rocket Vulkan Render");
    ImGui::ColorEdit3("clear color", (float*)&clearColor);
    ImGui::Separator();
    bool init = ImGui::Button("Init");
    bool walk = ImGui::Button("Walk"); 
    bool rotation = ImGui::Button("Rotation");
    ImGui::Separator();
    bool response = ImGui::Button("Response");
    ImGui::Separator();
    const char* items[] = { "motor 01", "motor 02", "motor 03", "motor 04", "motor 05", "motor 06", "motor 07", "motor 08", "motor 09", "motor 10"};
    ImGui::Combo("combo", &motor_id, items, IM_ARRAYSIZE(items));
    bool get_motor_data = ImGui::Button("Get Data");
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::InputFloat("Motor", &motor_data[motor_id], 0.001);
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    bool set_motor_data = ImGui::Button("Set Data");
    ImGui::Separator();
    ImGui::Checkbox("Show Robot State", &showRobotState);
    ImGui::Text("Robot Data");
    ImGui::InputDouble("W(mm)", &W, 0.001);
    ImGui::InputDouble("L(mm)", &L, 0.001);
    ImGui::InputDouble("h(mm)", &h, 0.001);
    ImGui::InputDouble("angle_x(°)", &angle_x, 0.001);
    ImGui::InputDouble("angle_y(°)", &angle_y, 0.001);
    ImGui::InputInt("direction", &direction, 1);
    ImGui::End();

    if(showRobotState)
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

        ImGui::PlotLines("Lines 01", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f)); ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); ImGui::Text("Progress Bar 01");
        
        ImGui::PlotLines("Lines 02", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f)); ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); ImGui::Text("Progress Bar 02");
        
        ImGui::PlotLines("Lines 03", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f)); ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); ImGui::Text("Progress Bar 03");
        
        ImGui::PlotLines("Lines 04", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f)); ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); ImGui::Text("Progress Bar 04");
        
        ImGui::PlotLines("Lines 05", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f)); ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); ImGui::Text("Progress Bar 05");
        
        ImGui::PlotLines("Lines 06", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f)); ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); ImGui::Text("Progress Bar 06");
        
        ImGui::PlotLines("Lines 07", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f)); ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); ImGui::Text("Progress Bar 07");
        
        ImGui::PlotLines("Lines 08", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f)); ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); ImGui::Text("Progress Bar 08");

        ImGui::End();
    }

    //ImGui::ShowDemoWindow();

    EventVarVec var;
    var.resize(2 + 10 + 10);

    if(response)
    {
        // Event Type
        var[0].type = Variant::TYPE_STRING_ID;
        var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "ui_event_response");
        // Action
        var[1].type = Variant::TYPE_STRING_ID;
        var[1].asStringId = 0;

        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }

    if(init || walk || rotation)
    {
        // Event Type
        var[0].type = Variant::TYPE_STRING_ID;
        var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "ui_event_logic");
        // Action
        var[1].type = Variant::TYPE_STRING_ID;
        var[1].asStringId = 0;
    }

    if(init) { var[1].asStringId = GlobalHashTable::HashString("StateMachine"_hash, "init_pos"); }
    if(walk) { var[1].asStringId = GlobalHashTable::HashString("StateMachine"_hash, "walk"); }
    if(rotation) { var[1].asStringId = GlobalHashTable::HashString("StateMachine"_hash, "rotation"); }

    for(int i = 0; i < 10 ; ++i)
    {
        var[2 + i].type = Variant::TYPE_FLOAT;
        var[2 + i].asFloat = motor_data[i];
    }

    for(int i = 0; i < 10 ; ++i)
    {
        var[12 + i].type = Variant::TYPE_INT32;
        var[12 + i].asFloat = valve_data[i];
    }

    if(init || walk || rotation)
    {
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
}