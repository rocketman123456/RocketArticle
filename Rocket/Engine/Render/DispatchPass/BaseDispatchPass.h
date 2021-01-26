#pragma once
#include "Interface/IDispatchPass.h"

namespace Rocket
{
    class BaseDispatchPass : implements IDispatchPass
    {
    public:
        void BeginPass(Frame&) override;
        void EndPass(Frame&) override;

    protected:
        BaseDispatchPass() = default;
    };
}
