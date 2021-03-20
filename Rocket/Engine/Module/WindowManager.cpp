#include "Module/WindowManager.h"
#include "Module/Application.h"
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_APPLE) || defined(PLATFORM_LINUX)
#include "GLFWWindow/WindowImplement.h"
#endif

using namespace Rocket;

WindowManager* Rocket::GetWindowManager() { return new WindowManager(); }

int WindowManager::Initialize()
{
    auto config = g_Application->GetConfig();

    WindowProps prop;
    prop.title = config->GetConfigInfo<String>("Graphics", "window_title");
    prop.width = config->GetConfigInfo<uint32_t>("Graphics", "window_width");
    prop.height = config->GetConfigInfo<uint32_t>("Graphics", "window_height");
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_APPLE) || defined(PLATFORM_LINUX)
    window_ = CreateRef<WindowImplement>(prop);
#else
	RK_CORE_ASSERT(false, "Unknown platform!");
#endif
    window_->Initialize();

    return 0;
}

void WindowManager::Finalize()
{
    window_->Finalize();
}

void WindowManager::Tick(Timestep ts)
{
    window_->Tick(ts);
}

bool WindowManager::OnWindowResize(EventPtr& e)
{
    window_->SetWidth(e->GetUInt32(1));
    window_->SetHeight(e->GetUInt32(2));
    return false;
}
