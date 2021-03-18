#pragma once
#include "Interface/IEvent.h"

namespace Rocket
{
	class AudioEvent : implements IEvent
	{
	public:
		AudioEvent(const std::string &name)
			: m_Name(name) {}
		const std::string &GetAudioName() const { return m_Name; }

		EVENT_CLASS_CATEGORY(EventCategoryAudio)
		EVENT_CLASS_TYPE(AudioEvent)
	private:
		std::string m_Name;
	};
} // namespace Rocket