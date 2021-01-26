#pragma once
#include "Common/Window.h"

struct GLFWwindow;

namespace Rocket
{
    class WindowImplement : implements Window
	{
	public:
		WindowImplement(const WindowProps &prop) : Window(prop) {}
		virtual ~WindowImplement() = default;

        virtual void Initialize() final;
		virtual void Finalize() final;

		virtual void* GetNativeWindow() const final { return m_Window; }
	private:
        WindowProps m_Props;
		GLFWwindow* m_Window;
	};
}