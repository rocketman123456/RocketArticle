#pragma once
#include "Math/GeomMath.h"

namespace Rocket
{
    // TODO : make float and double version of physics
    class Particle
    {
    public:

    protected:
        Vector3f position;
        Vector3f velocity;
        Vector3f acceleration;
        Real damping;
    };
}