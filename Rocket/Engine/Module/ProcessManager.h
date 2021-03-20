#pragma once
#include "Interface/IRuntimeModule.h"
#include "Process/Process.h"

#include <list>

namespace Rocket
{
    class ProcessManager : implements IRuntimeModule
    {
    public:
        RUNTIME_MODULE_TYPE(ProcessManager);
        ProcessManager() = default;
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
        uint32_t GetProcessCount(void) const { return process_list_.size(); }
    private:
        typedef std::list<StrongProcessPtr> ProcessList;
        ProcessList process_list_;
        uint64_t success_count_ = 0;
        uint64_t fail_count_ = 0;
    };

    ProcessManager* GetProcessManager();
    extern ProcessManager* g_ProcessManager;
} // namespace Rocket