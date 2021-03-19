#pragma once
#include "Core/Core.h"
#include "Utils/Timestep.h"

namespace Rocket
{
    struct WindowProps
	{
		String title;
		int32_t width;
		int32_t height;

		WindowProps(const String& title_ = "Rocket Engine",
					uint32_t width_ = 1280,
					uint32_t height_ = 720)
			: title(title_), width(width_), height(height_) {}
	};

    // Interface representing a desktop system based Window
	Interface Window
	{
	public:
		Window(WindowProps prop) : props_(prop) {}
		virtual ~Window() = default;

        virtual void Initialize() = 0;
        virtual void Finalize() = 0;
		virtual void Tick(Timestep ts) = 0;

		void SetWidth(uint32_t w) { props_.width = w; }
		void SetHeight(uint32_t h) { props_.height = h; }
		uint32_t GetWidth() const { return props_.width; }
		uint32_t GetHeight() const { return props_.height; }

		virtual void* GetNativeWindow() const = 0;

		static Ref<Window> Create(const WindowProps &props = WindowProps());
	protected:
		WindowProps props_;
	};
}