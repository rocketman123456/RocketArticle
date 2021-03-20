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

void UI::AddContext(const Ref<UIContext>& context) 
{ 
    contexts_.push_back(context); 
}

void UI::RemoveContext(const String& name)
{
    for(auto it = contexts_.begin(); it != contexts_.end(); ++it)
    {
        if(it->get()->name == name)
        {
            // TODO : remove ui context
            break;
        }
    }
}

void UI::DrawUI()
{
    for(auto context : contexts_)
    {
        context->Draw();
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