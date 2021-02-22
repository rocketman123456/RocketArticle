#pragma once
#include "Render/DispatchPass/BaseDispatchPass.h"

namespace Rocket
{
    class BRDFPrepare : implements BaseDispatchPass
    {
        void Dispatch(Frame& frame);
    };
}