#include "Render/DrawBasic/UI.h"
#include "Module/EventManager.h"

#include <imgui_internal.h>
#include <algorithm>

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
    const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIIIIII", "JJJJ", "KKKKKKK" };
    static int item_current = 0;
    ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
    ImGui::Checkbox("Show Robot State", &showRobotState);
    ImGui::Text("Robot Data");
    ImGui::InputDouble("angle_x", &angle_x, 0.001);
    ImGui::InputDouble("angle_y", &angle_y, 0.001);
    ImGui::InputInt("direction", &direction, 1);
    ImGui::End();

    if(showRobotState)
    {
        ImGui::Begin("Robot State", &showRobotState);
        static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
        ImGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));

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
        ImGui::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));

        static float progress = 0.0f, progress_dir = 1.0f;
        progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
        if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
        if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("Progress Bar");

        float progress_saturated = std::clamp(progress, 0.0f, 1.0f);
        char buf[32];
        sprintf(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);
        ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);

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