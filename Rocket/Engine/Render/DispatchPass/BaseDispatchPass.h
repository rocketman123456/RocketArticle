#pragma once
#include "Interface/IDispatchPass.hpp"

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
