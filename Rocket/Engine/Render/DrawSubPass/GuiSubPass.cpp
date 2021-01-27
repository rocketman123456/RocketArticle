#include "Render/DrawSubPass/GuiSubPass.h"

#include <imgui.h>
#include <glfw/glfw3.h>
namespace Rocket
{
    void GuiSubPass::Draw(Frame& frame)
    {
        if (ImGui::GetCurrentContext())
        {
            ImGui::NewFrame();

            //ImGui::ShowDemoWindow();
            {
                ImGui::Begin("Hello, world!");
                ImGui::Text("This is some useful text.");
                ImGui::End();
            }

            ImGui::Render();
        }
    }
}