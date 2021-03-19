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
    m_Window = CreateRef<WindowImplement>(prop);
#else
	RK_CORE_ASSERT(false, "Unknown platform!");
#endif
    m_Window->Initialize();

    return 0;
}

void WindowManager::Finalize()
{
    m_Window->Finalize();
}

void WindowManager::Tick(Timestep ts)
{
    m_Window->Tick(ts);
}

bool WindowManager::OnWindowResize(EventPtr& e)
{
    m_Window->SetWidth(e->GetUInt32(1));
    m_Window->SetHeight(e->GetUInt32(2));
    return false;
}
