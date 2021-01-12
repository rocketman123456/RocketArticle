#pragma once
#include "Common/Window.h"

#include <GLFW/glfw3.h>

namespace Rocket
{
    class WindowImplement : implements Window
	{
	public:
		WindowImplement(const WindowProps &props) : m_Props(props) {}
		virtual ~WindowImplement() = default;

        virtual void Initialize() final;
		virtual void Finalize() final;

		virtual void PollEvent() final;
		virtual void Update() final;

		uint32_t GetWidth() const final { return m_Data.Width; }
		uint32_t GetHeight() const final { return m_Data.Height; }

		// Window attributes
		void SetEventCallback(const EventCallbackFn &callback) final { m_Data.EventCallback = callback; }
		bool IsVSync() const final { return m_Data.VSync; }
		void SetVSync(bool enabled) final;

		virtual void* GetNativeWindow() const final { return m_Window; }
    private:
		virtual void SetCallback();

	private:
		WindowData m_Data;
        WindowProps m_Props;
		GLFWwindow* m_Window;
	};
}