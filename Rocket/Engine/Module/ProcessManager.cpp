#include "Module/ProcessManager.h"

using namespace Rocket;

ProcessManager* Rocket::GetProcessManager()
{
    return new ProcessManager();
}

int ProcessManager::Initialize()
{
    return 0;
}

void ProcessManager::Finalize()
{
    AbortAllProcesses(true);
    m_ProcessList.clear();
}

void ProcessManager::Tick(Timestep ts)
{
    PROFILE_BEGIN_CPU_SAMPLE(ProcessManagerUpdate, 0);

    auto result_i = UpdateProcesses(ts.GetMilliseconds());
    //RK_CORE_TRACE("Process Success {0}, Fail {1}", m_SuccessCount, m_FailCount);

    PROFILE_END_CPU_SAMPLE();
}

//---------------------------------------------------------------------------------------------------------------------
// The process update tick.  Called every logic tick.  This function returns the number of process chains that
// succeeded in the upper 32 bits and the number of process chains that failed or were aborted in the lower 32 bits.
//---------------------------------------------------------------------------------------------------------------------
uint64_t ProcessManager::UpdateProcesses(unsigned long deltaMs)
{
    uint64_t successCount = 0;
    uint64_t failCount = 0;

    ProcessList::iterator it = m_ProcessList.begin();
    while (it != m_ProcessList.end())
    {
        // grab the next process
        StrongProcessPtr pCurrProcess = (*it);

        // save the iterator and increment the old one in case we need to remove this process from the list
        ProcessList::iterator thisIt = it;
        ++it;

        // process is uninitialized, so initialize it
        if (pCurrProcess->GetState() == Process::UNINITIALIZED)
            pCurrProcess->OnInit();

        // give the process an update tick if it's running
        if (pCurrProcess->GetState() == Process::RUNNING)
            pCurrProcess->OnUpdate(deltaMs);

        // check to see if the process is dead
        if (pCurrProcess->IsDead())
        {
            // run the appropriate exit function
            switch (pCurrProcess->GetState())
            {
            case Process::SUCCEEDED:
            {
                pCurrProcess->OnSuccess();
                StrongProcessPtr pChild = pCurrProcess->RemoveChild();
                if (pChild)
                    AttachProcess(pChild);
                else
                    ++successCount; // only counts if the whole chain completed
                break;
            }

            case Process::FAILED:
            {
                pCurrProcess->OnFail();
                ++failCount;
                break;
            }

            case Process::ABORTED:
            {
                pCurrProcess->OnAbort();
                ++failCount;
                break;
            }

            default:
                break;
            }

            // remove the process and destroy it
            m_ProcessList.erase(thisIt);
        }
    }

    m_SuccessCount = successCount;
    m_FailCount = failCount;
    return (static_cast<uint64_t>(successCount << 32) | static_cast<uint64_t>(failCount));
}

//---------------------------------------------------------------------------------------------------------------------
// Attaches the process to the process list so it can be run on the next update.
//---------------------------------------------------------------------------------------------------------------------
WeakProcessPtr ProcessManager::AttachProcess(StrongProcessPtr pProcess)
{
    m_ProcessList.push_front(pProcess);
    return WeakProcessPtr(pProcess);
}

//---------------------------------------------------------------------------------------------------------------------
// Aborts all processes.  If immediate == true, it immediately calls each ones OnAbort() function and destroys all
// the processes.
//---------------------------------------------------------------------------------------------------------------------
void ProcessManager::AbortAllProcesses(bool immediate)
{
    ProcessList::iterator it = m_ProcessList.begin();
    while (it != m_ProcessList.end())
    {
        ProcessList::iterator tempIt = it;
        ++it;

        StrongProcessPtr pProcess = *tempIt;
        if (pProcess->IsAlive())
        {
            pProcess->SetState(Process::ABORTED);
            if (immediate)
            {
                pProcess->OnAbort();
                m_ProcessList.erase(tempIt);
            }
        }
    }
}

void ProcessManager::PauseAllProcesses()
{
    ProcessList::iterator it = m_ProcessList.begin();
    while (it != m_ProcessList.end())
    {
        StrongProcessPtr pProcess = *it;
        if (pProcess->IsAlive())
        {
            pProcess->Pause();
        }
        ++it;
    }
}

void ProcessManager::UnPauseAllProcesses()
{
    ProcessList::iterator it = m_ProcessList.begin();
    while (it != m_ProcessList.end())
    {
        StrongProcessPtr pProcess = *it;
        if (pProcess->IsAlive())
        {
            pProcess->UnPause();
        }
        ++it;
    }
}
