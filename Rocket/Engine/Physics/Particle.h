#pragma once
#include "Math/GeomMath.h"

namespace Rocket
{
    // TODO : make z upward direction
    // TODO : make float and double version of physics
    class Particle
    {
    public:
        void Integrate(Real duration);
        void ClearAccumulator();

    protected:
        Vector3f position_;
        Vector3f velocity_;
        Vector3f acceleration_;
        Vector3f force_accumlate_;
        Real damping_;
        Real inverse_mass_;
    };
}