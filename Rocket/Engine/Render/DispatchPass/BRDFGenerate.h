#pragma once
#include "Render/DispatchPass/BaseDispatchPass.h"

namespace Rocket
{
    class BRDFGenerate : implements BaseDispatchPass
    {
        void Dispatch(Frame& frame);
    };
}