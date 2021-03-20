#include "Process/Process.h"

using namespace Rocket;

Process::Process(void)
{
    state_ = UNINITIALIZED;
    child_.reset();
}

Process::~Process(void)
{
    if (child_)
    {
        child_->OnAbort();
    }
}

StrongProcessPtr Process::RemoveChild(void)
{
    if (child_)
    {
        StrongProcessPtr pChild = child_; // this keeps the child from getting destroyed when we clear it
        child_.reset();
        return pChild;
    }

    return StrongProcessPtr();
}
