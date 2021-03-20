#pragma once
#include "Core/Core.h"

namespace Rocket
{
    class Process;
    using StrongProcessPtr = Ref<Process>;
    using WeakProcessPtr = StoreRef<Process>;

    //---------------------------------------------------------------------------------------------------------------------
    // Process class
    //
    // Processes are ended by one of three methods: Success, Failure, or Aborted.
    //		- Success means the process completed successfully.  If the process has a child, it will be attached to
    //		  the process mgr.
    //		- Failure means the process started but failed in some way.  If the process has a child, it will be
    //		  aborted.
    //		- Aborted processes are processes that are canceled while not submitted to the process mgr.  Depending
    //		  on the circumstances, they may or may not have gotten an OnInit() call.  For example, a process can
    //		  spawn another process and call AttachToParent() on itself.  If the new process fails, the child will
    //		  get an Abort() call on it, even though its status is RUNNING.
    //---------------------------------------------------------------------------------------------------------------------
    Interface Process
    {
        friend class ProcessManager;
    public:
        enum State
        {
            // Processes that are neither dead nor alive
            UNINITIALIZED = 0, // created but not running
            REMOVED,           // removed from the process list but not destroyed; 
                               // this can happen when a process that is already running is parented to another process

            // Living processes
            RUNNING, // initialized and running
            PAUSED,  // initialized but paused

            // Dead processes
            SUCCEEDED, // completed successfully
            FAILED,    // failed to complete
            ABORTED,   // aborted; may not have started
        };

    private:
        State state_;             // the current state of the process
        StrongProcessPtr child_; // the child process, if any

    public:
        // construction
        Process(void);
        virtual ~Process(void);

    protected:
        // interface; these functions should be overridden by the subclass as needed
        virtual void OnInit(void) { state_ = RUNNING; }  // called during the first update; responsible for setting the initial state (typically RUNNING)
        virtual void OnUpdate(unsigned long deltaMs) = 0; // called every frame
        virtual void OnSuccess(void) {}                   // called if the process succeeds (see below)
        virtual void OnFail(void) {}                      // called if the process fails (see below)
        virtual void OnAbort(void) {}                     // called if the process is aborted (see below)

    public:
        // Functions for ending the process.
        inline void Succeed(void);
        inline void Fail(void);

        // pause
        inline void Pause(void);
        inline void UnPause(void);

        // accessors
        State GetState(void) const { return state_; }
        bool IsAlive(void) const { return (state_ == RUNNING || state_ == PAUSED); }
        bool IsDead(void) const { return (state_ == SUCCEEDED || state_ == FAILED || state_ == ABORTED); }
        bool IsRemoved(void) const { return (state_ == REMOVED); }
        bool IsPaused(void) const { return state_ == PAUSED; }

        // child functions
        inline void AttachChild(StrongProcessPtr pChild);
        StrongProcessPtr RemoveChild(void);                   // releases ownership of the child
        StrongProcessPtr PeekChild(void) { return child_; } // doesn't release ownership of the child

    private:
        void SetState(State newState) { state_ = newState; }
    };

    //---------------------------------------------------------------------------------------------------------------------
    // Inline function definitions
    //---------------------------------------------------------------------------------------------------------------------
    inline void Process::Succeed(void)
    {
        RK_CORE_ASSERT(state_ == RUNNING || state_ == PAUSED, "Process Success");
        state_ = SUCCEEDED;
    }

    inline void Process::Fail(void)
    {
        RK_CORE_ASSERT(state_ == RUNNING || state_ == PAUSED, "Process Fail");
        state_ = FAILED;
    }

    inline void Process::AttachChild(StrongProcessPtr pChild)
    {
        if (child_)
            child_->AttachChild(pChild);
        else
            child_ = pChild;
    }

    inline void Process::Pause(void)
    {
        if (state_ == RUNNING)
            state_ = PAUSED;
        else
            RK_CORE_WARN("Attempting to pause a process that isn't running");
    }

    inline void Process::UnPause(void)
    {
        if (state_ == PAUSED)
            state_ = RUNNING;
        else
            RK_CORE_WARN("Attempting to unpause a process that isn't paused");
    }
} // namespace Rocket