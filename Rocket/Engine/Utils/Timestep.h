#pragma once

namespace Rocket
{
	class Timestep
	{
	public:
		Timestep(float time = 0.0f) : m_Time(time) {}

		operator float() const { return m_Time; }

		float GetSeconds() const { return m_Time * 0.001f; }
		float GetMilliseconds() const { return m_Time; }

	private:
		float m_Time;
	};
} // namespace Rocket