#include "Utils/Timer.h"

namespace Rocket
{
    void ElapseTimer::InitTime()
    {
        m_StartTimepoint = std::chrono::high_resolution_clock::now();
        m_CurrentTimepoint = std::chrono::high_resolution_clock::now();
        m_TimeLastTick = 1000;
    }

    void ElapseTimer::MarkTimeThisTick()
    {
        m_ElapsedTimepoint = std::chrono::high_resolution_clock::now();
        m_CurrentTimepoint = m_ElapsedTimepoint;
    }

    double ElapseTimer::GetElapsedTime(void)
    {
        double duration = GetElapsedTimeCount() * 0.001f;
        return duration;
    }

    uint64_t ElapseTimer::GetElapsedTimeCount(void)
    {
        m_ElapsedTimepoint = std::chrono::high_resolution_clock::now();
        uint64_t start = TIMER_COUNT(m_CurrentTimepoint);
        uint64_t end = TIMER_COUNT(m_ElapsedTimepoint);

        m_TimeLastTick = end - start;

        // Ignore too small time to avoid error
        if (m_TimeLastTick <= 2)
            m_TimeLastTick = 2;

        return m_TimeLastTick;
    }

    double ElapseTimer::GetExactTime(void)
    {
        double duration = GetExactTimeCount() * 0.001f;
        return duration;
    }

    uint64_t ElapseTimer::GetExactTimeCount(void)
    {
        m_CurrentTimepoint = std::chrono::high_resolution_clock::now();
        uint64_t start = TIMER_COUNT(m_StartTimepoint);
        uint64_t end = TIMER_COUNT(m_CurrentTimepoint);
        
        uint64_t duration = (end - start);
        return duration;
    }
} // namespace Rocket
