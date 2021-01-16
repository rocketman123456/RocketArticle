#pragma once
#include "Interface/IRuntimeModule.h"
#include "Process/Process.h"

#include <list>

namespace Rocket
{
    class ProcessManager : implements IRuntimeModule
    {
    public:
        ProcessManager() : IRuntimeModule("ProcessManager") {}
        virtual ~ProcessManager() = default;

        virtual int Initialize() override;
        virtual void Finalize() override;

        virtual void Tick(Timestep ts) override;

        // interface
        uint64_t UpdateProcesses(unsigned long deltaMs);         // updates all attached processes
        WeakProcessPtr AttachProcess(StrongProcessPtr pProcess); // attaches a process to the process mgr
        void AbortAllProcesses(bool immediate);
        void PauseAllProcesses();
        void UnPauseAllProcesses();

        // accessors
        uint32_t GetProcessCount(void) const { return m_ProcessList.size(); }
    private:
        typedef std::list<StrongProcessPtr> ProcessList;
        ProcessList m_ProcessList;
    };

    ProcessManager* GetProcessManager();
    extern ProcessManager* g_ProcessManager;
} // namespace Rocket