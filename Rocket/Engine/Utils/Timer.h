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
	ElapseTimer() : start_timepoint_{ Clock::now() }, lap_timepoint_{ Clock::now() } {}
	virtual ~ElapseTimer() = default;

	void Start(void)
	{
		if (!running_)
		{
			running_ = true;
			start_timepoint_ = Clock::now();
		}
	}

	void MarkLapping(void)
	{
		lapping_ = true;
		lap_timepoint_ = Clock::now();
	}

	bool IsRunning() const { return running_; }

	template <typename T = DefaultResolution>
	double GetExactTime(void)
	{
		auto duration = std::chrono::duration<double, T>(Clock::now() - start_timepoint_);
		return duration.count();
	}

	template <typename T = DefaultResolution>
	double Stop()
	{
		if (!running_)
		{
			return 0;
		}

		running_ = false;
		lapping_ = false;
		auto duration = std::chrono::duration<double, T>(Clock::now() - start_timepoint_);
		start_timepoint_ = Clock::now();
		lap_timepoint_ = Clock::now();

		return duration.count();
	}

	template <typename T = DefaultResolution>
	double GetElapsedTime()
	{
		if (!running_)
			return 0;

		Clock::time_point start = start_timepoint_;

		if (lapping_)
			start = lap_timepoint_;

		return std::chrono::duration<double, T>(Clock::now() - start).count();
	}

	template <typename T = DefaultResolution>
	double GetTickTime()
	{
		auto now = Clock::now();
		auto duration = std::chrono::duration<double, T>(now - previous_tick_);
		previous_tick_ = now;
		return duration.count();
	}

private:
	Clock::time_point start_timepoint_;
	Clock::time_point lap_timepoint_;
	Clock::time_point previous_tick_;
	bool running_ { false };
	bool lapping_ { false };
};
