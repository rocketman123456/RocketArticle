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

        void Initialize() final;
		void Finalize() final;
		void Tick(Timestep ts) final;

		void* GetNativeWindow() const final { return window_; }
	private:
        //WindowProps m_Props;
		GLFWwindow* window_;
	};
}