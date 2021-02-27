#include "Utils/Timer.h"

using namespace Rocket;

void ElapseTimer::Start()
{
    if (!m_Running)
    {
        m_Running = true;
        m_StartTimepoint = Clock::now();
    }
}

void ElapseTimer::MarkLapping()
{
    m_Lapping = true;
    m_LapTimePoint = Clock::now();
}
