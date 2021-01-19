#pragma once
#include "Module/GraphicsManager.h"

struct GLFWwindow;

namespace Rocket
{
    class OpenGLGraphicsManager : implements GraphicsManager
    {
    public:
        OpenGLGraphicsManager() : GraphicsManager("OpenGLGraphicsManager") {}
        virtual ~OpenGLGraphicsManager() = default;

        virtual int Initialize() final;
        virtual void Finalize() final;

        virtual void Tick(Timestep ts) final;

    protected:
        virtual void SwapBuffers() final;

    private:
        GLFWwindow* m_WindowHandle = nullptr;
        bool m_VSync = true;
    };
}