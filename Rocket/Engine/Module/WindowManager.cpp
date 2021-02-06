#include "Module/WindowManager.h"
#include "Module/MemoryManager.h"
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_APPLE) || defined(PLATFORM_LINUX)
#include "GLFWWindow/WindowImplement.h"
#endif

using namespace Rocket;

WindowManager* Rocket::GetWindowManager()
{
    return new WindowManager();
}

int WindowManager::Initialize()
{
    WindowProps prop;
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
