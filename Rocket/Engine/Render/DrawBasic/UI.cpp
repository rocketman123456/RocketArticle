#include "Render/DrawBasic/UI.h"
#include "Module/EventManager.h"

#include <imgui_internal.h>

using namespace Rocket;

void UI::UpdataOverlay(uint32_t width, uint32_t height)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGuiContext* context = ImGui::GetCurrentContext();

    ImGui::NewFrame();

    EventVarVec var;
    var.resize(2);
    // Event Type
    var[0].type = Variant::TYPE_STRING_ID;
    var[0].m_asStringId = EventHashTable::HashString("ui_event_logic");
    // Action
    var[1].type = Variant::TYPE_STRING_ID;
    var[1].m_asStringId = 0;

    ImGui::Begin("Rocket", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Hello, world!");
    ImGui::Text("Rocket Vulkan Render");
    ImGui::ColorEdit3("clear color", (float*)&clearColor);
    bool init = ImGui::Button("Init");
    bool walk = ImGui::Button("Walk");
    bool rotation = ImGui::Button("Rotation");
    if(init)
    {
        var[1].m_asStringId = EventHashTable::HashString("init_pos");
    }
    if(walk)
    {
        var[1].m_asStringId = EventHashTable::HashString("init_pos");
    }
    if(rotation)
    {
        var[1].m_asStringId = EventHashTable::HashString("init_pos");
    }
    ImGui::End();

    if(init || walk || rotation)
    {
        EventPtr event = CreateRef<Event>(var);
        g_EventManager->QueueEvent(event);
    }

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