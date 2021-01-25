#pragma once
#include "Core/Core.h"

#include <chrono>

namespace Rocket
{
    #define TIMER_COUNT(x) std::chrono::time_point_cast<std::chrono::microseconds>(x)\
            .time_since_epoch().count()
    // TODO : make timer thread safe
	class ElapseTimer
	{
	public:
		void InitTime(void);
		void MarkTimeThisTick(void);
		double GetElapsedTime(void);
        uint64_t GetElapsedTimeCount(void);
		double GetExactTime(void);
		uint64_t GetExactTimeCount(void);
		inline uint64_t GetTickRate() { return 1000; }

	private:
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
		std::chrono::time_point<std::chrono::steady_clock> m_CurrentTimepoint;
        std::chrono::time_point<std::chrono::steady_clock> m_ElapsedTimepoint;
		uint64_t m_TimeLastTick;
	};

    extern ElapseTimer* g_GlobalTimer;
} // namespace Rocket