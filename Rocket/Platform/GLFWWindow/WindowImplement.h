#pragma once
#include "Common/Window.h"

struct GLFWwindow;

namespace Rocket
{
    class WindowImplement : implements Window
	{
	public:
		WindowImplement(const WindowProps &props) : m_Props(props) {}
		virtual ~WindowImplement() = default;

        virtual void Initialize() final;
		virtual void Finalize() final;

		virtual void Tick() final;

		uint32_t GetWidth() const final { return m_Data.Width; }
		uint32_t GetHeight() const final { return m_Data.Height; }

		// Window attributes
		void SetEventCallback(const EventCallbackFn &callback) final { m_Data.EventCallback = callback; }

		virtual void* GetNativeWindow() const final { return m_Window; }
    private:
		virtual void SetCallback();

	private:
		WindowData m_Data;
        WindowProps m_Props;
		GLFWwindow* m_Window;
	};
}