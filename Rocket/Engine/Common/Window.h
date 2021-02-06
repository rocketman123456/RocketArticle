#pragma once
#include "Core/Core.h"

namespace Rocket
{
    struct WindowProps
	{
		String Title;
		uint32_t Width;
		uint32_t Height;

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

		uint32_t GetWidth() const { return m_Props.Width; }
		uint32_t GetHeight() const { return m_Props.Height; }

		virtual void* GetNativeWindow() const = 0;

		static Ref<Window> Create(const WindowProps &props = WindowProps());
	protected:
		WindowProps m_Props;
	};
}