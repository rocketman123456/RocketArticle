#include "Module/WindowManager.h"

namespace Rocket
{
    WindowManager* GetWindowManager()
    {
        return new WindowManager();
    }

    int WindowManager::Initialize()
    {
        WindowProps prop;
        m_Window = Window::Create(prop);
        m_Window->Initialize();

        return 0;
    }

    void WindowManager::Finalize()
    {
        m_Window->Finalize();
    }

    void WindowManager::Tick(Timestep ts)
    {
        PROFILE_BEGIN_CPU_SAMPLE(WindowManagerUpdate, 0);

        m_Window->Tick();

        PROFILE_END_CPU_SAMPLE();
    }
}