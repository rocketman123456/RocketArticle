#pragma once
#include "Core/Core.h"

namespace Rocket
{
    #define TIMER_COUNT(x) std::chrono::time_point_cast<std::chrono::microseconds>(x)\
            .time_since_epoch().count()

	class ProfilerTimer
	{
	public:
		void InitTime(void);
		void MarkTimeThisTick(void);
		float GetElapsedTime(void);
        uint64_t GetElapsedTimeCount(void);
		float GetExactTime(void);
		uint64_t GetExactTimeCount(void);
		inline uint64_t GetTickRate() { return 1000; }

	private:
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
		std::chrono::time_point<std::chrono::steady_clock> m_CurrentTimepoint;
        std::chrono::time_point<std::chrono::steady_clock> m_ElapsedTimepoint;
		long long m_TimeLastTick;
	};
} // namespace Rocket