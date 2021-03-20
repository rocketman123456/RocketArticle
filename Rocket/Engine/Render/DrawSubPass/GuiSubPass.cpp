#include "Render/DrawSubPass/GuiSubPass.h"
#include "Module/GraphicsManager.h"

#include <imgui.h>
//#include <glfw/glfw3.h>

using namespace Rocket;

void GuiSubPass::Draw(Frame& frame)
{
    if (ImGui::GetCurrentContext())
    {
        //ImGui::NewFrame();
    
        // Count FPS
        {
            double Duration = timer_.GetTickTime();
    
            count_fps_++;
            count_time_ += Duration;
    
            if (count_time_ >= 1000.0f)
            {
                fps_ = count_fps_;
                count_fps_ = 0;
                count_time_ = 0.0f;
            }
        }
        
        // Draw GUI
        //{
        //    ImGui::Begin("Hello, world!");
        //    ImGui::Text("FPS : %d", fps_);
    
        //    //auto frame_buffer = g_GraphicsManager->GetFrameBuffer("Draw2D Buffer");
        //    //if (frame_buffer)
        //    //{
        //    //    uint32_t texture_id = frame_buffer->GetColorAttachmentRendererID(0);
        //    //    if (texture_id)
        //    //    {
        //    //        auto spec = frame_buffer->GetSpecification();
        //    //        float width = spec.color_width;
        //    //        float height = spec.color_height;
        //    //        ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2{ width, height }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        //    //    }
        //    //}
        //    
        //    ImGui::End();
        //}
    
        //ImGui::Render();
        // Update and Render additional Platform Windows
        //ImGuiIO& io = ImGui::GetIO();
        //if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        //{
        //    ImGui::UpdatePlatformWindows();
        //    ImGui::RenderPlatformWindowsDefault();
        //}
    }
}
