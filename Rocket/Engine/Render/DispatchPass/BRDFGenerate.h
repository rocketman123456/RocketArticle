#pragma once
#include "Render/DispatchPass/BaseDispatchPass.h"

namespace Rocket
{
    class BRDFGenerate : implements BaseDispatchPass
    {
    public:
        void Dispatch(Frame& frame);
    };
}