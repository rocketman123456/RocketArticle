#pragma once
#include "Core/Core.h"

namespace Rocket
{
    struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string &title = "Rocket Engine",
					uint32_t width = 1280,
					uint32_t height = 720)
			: Title(title), Width(width), Height(height) {}
	};

    // Interface representing a desktop system based Window
	Interface Window
	{
	public:
		virtual ~Window() = default;

        virtual void Initialize() = 0;
        virtual void Finalize() = 0;

		virtual void Tick() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Ref<Window> Create(const WindowProps &props = WindowProps());
	};
}