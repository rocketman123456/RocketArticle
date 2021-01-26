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
}