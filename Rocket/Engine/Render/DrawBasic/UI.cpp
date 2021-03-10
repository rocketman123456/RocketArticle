#include "Render/DrawBasic/UI.h"
#include "Module/EventManager.h"

#include <imgui_internal.h>

using namespace Rocket;

void UI::Initialize()
{
    // Setup ImGui
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;

		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking
		// TODO : fix multi-viewort error
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
        if(1)
        {
		    ImGui::StyleColorsDark();
        }
        else
        {
		    ImGui::StyleColorsClassic();
        }

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows 
		// can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
	}
}

void UI::Finalize()
{
    ImGui::DestroyContext();
}

void UI::DrawUI()
{
    EventVarVec var;
    var.resize(5);
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
    ImGui::Checkbox("Show Robot State", &showRobotState);
    ImGui::Text("Robot Data");
    ImGui::InputDouble("angle_x", &angle_x, 0.001);
    ImGui::InputDouble("angle_y", &angle_y, 0.001);
    ImGui::InputInt("direction", &direction, 1);
    ImGui::End();

    if(showRobotState)
    {
        ImGui::Begin("Robot State", &showRobotState);
        ImGui::End();
    }

    ImGui::ShowDemoWindow();

    var[2].type = Variant::TYPE_DOUBLE;
    var[2].asDouble = angle_x;
    var[3].type = Variant::TYPE_DOUBLE;
    var[3].asDouble = angle_y;
    var[3].type = Variant::TYPE_INT32;
    var[3].asDouble = direction;

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