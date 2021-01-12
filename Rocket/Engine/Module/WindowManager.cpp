#include "Module/WindowManager.h"

namespace Rocket
{
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
        m_Window->Update();
    }
}