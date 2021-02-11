#include "Render/DrawSubPass/GuiSubPass.h"
#include "Module/GraphicsManager.h"

#include <imgui.h>
//#include <glfw/glfw3.h>

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

            auto frame_buffer = g_GraphicsManager->GetFrameBuffer("Draw2D Buffer");
            if (frame_buffer)
            {
                uint32_t texture_id = frame_buffer->GetColorAttachmentRendererID(0);
                if (texture_id)
                {
                    auto spec = frame_buffer->GetSpecification();
                    float width = spec.ColorWidth;
                    float height = spec.ColorHeight;
                    ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2{ width, height }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
                }
            }
            
            ImGui::Text("FPS : %d", m_FPS);
            ImGui::End();
        }

        ImGui::Render();
    }
}
