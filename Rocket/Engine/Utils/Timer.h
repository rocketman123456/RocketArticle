#pragma once
#include "Core/Core.h"

#include <chrono>

#define TIMER_COUNT(x) std::chrono::time_point_cast<std::chrono::microseconds>(x).time_since_epoch().count()
// TODO : make timer thread safe
// TODO : use this timer avoid thread problem
class ElapseTimer
{
public:
	using Seconds = std::ratio<1>;
	using Milliseconds = std::ratio<1, 1000>;
	using Microseconds = std::ratio<1, 1000000>;
	using Nanoseconds = std::ratio<1, 1000000000>;

	// Configure
	using Clock = std::chrono::steady_clock;
	using DefaultResolution = Milliseconds;

public:
	ElapseTimer() : m_StartTimepoint{ Clock::now() }, m_LapTimePoint{ Clock::now() } {}
	virtual ~ElapseTimer() = default;

	void Start(void)
	{
		if (!m_Running)
		{
			m_Running = true;
			m_StartTimepoint = Clock::now();
		}
	}

	void MarkLapping(void)
	{
		m_Lapping = true;
		m_LapTimePoint = Clock::now();
	}

	bool IsRunning() const { return m_Running; }

	template <typename T = DefaultResolution>
	double GetExactTime(void)
	{
		auto duration = std::chrono::duration<double, T>(Clock::now() - m_StartTimepoint);
		return duration.count();
	}

	template <typename T = DefaultResolution>
	double Stop()
	{
		if (!m_Running)
		{
			return 0;
		}

		m_Running = false;
		m_Lapping = false;
		auto duration = std::chrono::duration<double, T>(Clock::now() - m_StartTimepoint);
		m_StartTimepoint = Clock::now();
		m_LapTimePoint = Clock::now();

		return duration.count();
	}

	template <typename T = DefaultResolution>
	double GetElapsedTime()
	{
		if (!m_Running)
			return 0;

		Clock::time_point start = m_StartTimepoint;

		if (m_Lapping)
			start = m_LapTimePoint;

		return std::chrono::duration<double, T>(Clock::now() - start).count();
	}

	template <typename T = DefaultResolution>
	double GetTickTime()
	{
		auto now = Clock::now();
		auto duration = std::chrono::duration<double, T>(now - m_PreviousTick);
		m_PreviousTick = now;
		return duration.count();
	}

private:
	Clock::time_point m_StartTimepoint;
	Clock::time_point m_LapTimePoint;
	Clock::time_point m_PreviousTick;
	bool m_Running { false };
	bool m_Lapping { false };
};
