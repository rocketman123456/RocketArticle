#include "Process/Process.h"

using namespace Rocket;

Process::Process(void)
{
    m_state = UNINITIALIZED;
    m_pChild.reset();
}

Process::~Process(void)
{
    if (m_pChild)
    {
        m_pChild->OnAbort();
    }
}

StrongProcessPtr Process::RemoveChild(void)
{
    if (m_pChild)
    {
        StrongProcessPtr pChild = m_pChild; // this keeps the child from getting destroyed when we clear it
        m_pChild.reset();
        return pChild;
    }

    return StrongProcessPtr();
}
