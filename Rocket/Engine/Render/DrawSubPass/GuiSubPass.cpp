#include "Render/DrawSubPass/GuiSubPass.h"

#include <imgui.h>
#include <glfw/glfw3.h>

using namespace Rocket;

void GuiSubPass::Draw(Frame& frame)
{
    if (ImGui::GetCurrentContext())
    {
        ImGui::NewFrame();

        // Count FPS
        {
            double Duration = m_Timer.GetTickTime();

            m_CountFrame++;
            m_CountTime += Duration;

            if (m_CountTime >= 1000.0f)
            {
                m_FPS = m_CountFrame;
                m_CountFrame = 0;
                m_CountTime = 0.0f;
            }
        }
        
        {
            ImGui::Begin("Hello, world!");
            //ImGui::Text("This is some useful text.");
            ImGui::Text("FPS : %d", m_FPS);
            ImGui::End();
        }

        ImGui::Render();
    }
}
