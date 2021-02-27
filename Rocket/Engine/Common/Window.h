#pragma once
#include "Core/Core.h"

namespace Rocket
{
    struct WindowProps
	{
		String Title;
		int32_t Width;
		int32_t Height;

		WindowProps(const String& title = "Rocket Engine",
					uint32_t width = 1280,
					uint32_t height = 720)
			: Title(title), Width(width), Height(height) {}
	};

    // Interface representing a desktop system based Window
	Interface Window
	{
	public:
		Window(WindowProps prop) : m_Props(prop) {}
		virtual ~Window() = default;

        virtual void Initialize() = 0;
        virtual void Finalize() = 0;

		void SetWidth(uint32_t w) { m_Props.Width = w; }
		void SetHeight(uint32_t h) { m_Props.Height = h; }
		uint32_t GetWidth() const { return m_Props.Width; }
		uint32_t GetHeight() const { return m_Props.Height; }

		virtual void* GetNativeWindow() const = 0;

		static Ref<Window> Create(const WindowProps &props = WindowProps());
	protected:
		WindowProps m_Props;
	};
}