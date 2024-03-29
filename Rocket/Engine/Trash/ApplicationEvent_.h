#pragma once
#include "Interface/IEvent.h"

namespace Rocket
{
	class WindowResizeEvent : implements IEvent
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height, float x, float y)
			: m_Width(width), m_Height(height), m_xScale(x), m_yScale(y) {}

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		float GetXScale() const { return m_xScale; }
		float GetYScale() const { return m_yScale; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "[" << TimeStamp << "] " << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		uint32_t m_Width, m_Height;
		float m_xScale, m_yScale;
	};

	class WindowCloseEvent : implements IEvent
	{
	public:
		WindowCloseEvent() = default;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "[" << TimeStamp << "] " << "WindowCloseEvent";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppTickEvent : implements IEvent
	{
	public:
		AppTickEvent() = default;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "[" << TimeStamp << "] " << "AppTickEvent";
			return ss.str();
		}

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppUpdateEvent : implements IEvent
	{
	public:
		AppUpdateEvent() = default;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "[" << TimeStamp << "] " << "AppUpdateEvent";
			return ss.str();
		}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppRenderEvent : implements IEvent
	{
	public:
		AppRenderEvent() = default;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "[" << TimeStamp << "] " << "AppRenderEvent";
			return ss.str();
		}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
} // namespace Rocket
