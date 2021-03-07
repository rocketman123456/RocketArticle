#include "Render/DrawBasic/UI.h"
#include "Module/EventManager.h"

#include <imgui_internal.h>

using namespace Rocket;

void UI::DrawUI()
{
    EventVarVec var;
    var.resize(4);
    // Event Type
    var[0].type = Variant::TYPE_STRING_ID;
    var[0].asStringId = EventHashTable::HashString("ui_event_logic");
    // Action
    var[1].type = Variant::TYPE_STRING_ID;
    var[1].asStringId = 0;

    ImGui::Begin("Rocket", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Hello, world!");
    ImGui::Text("Rocket Vulkan Render");
    ImGui::ColorEdit3("clear color", (float*)&clearColor);
    bool init = ImGui::Button("Init");
    bool walk = ImGui::Button("Walk");
    bool rotation = ImGui::Button("Rotation");
    double angle_x;
    double angle_y;
    ImGui::InputDouble("angle_x", &angle_x);
    ImGui::InputDouble("angle_y", &angle_y);
    ImGui::End();

    var[2].type = Variant::TYPE_DOUBLE;
    var[2].asDouble = angle_x;
    var[3].type = Variant::TYPE_DOUBLE;
    var[3].asDouble = angle_y;

    if(init)
    {
        var[1].asStringId = StateMachineHashTable::HashString("init_pos");
    }
    if(walk)
    {
        var[1].asStringId = StateMachineHashTable::HashString("walk");
    }
    if(rotation)
    {
        var[1].asStringId = StateMachineHashTable::HashString("rotation");
    }

    if(init || walk || rotation)
    {
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }
}

void UI::UpdataOverlay(uint32_t width, uint32_t height)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGuiContext* context = ImGui::GetCurrentContext();

    ImGui::NewFrame();

    DrawUI();

    ImGui::EndFrame();
    ImGui::Render();

    // Update and Render additional Platform Windows
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

bool UI::OnUIResponse(EventPtr& e)
{
    return false;
}