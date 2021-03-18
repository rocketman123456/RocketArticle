#pragma once
#include "Math/GeomMath.h"

namespace Rocket
{
    std::tuple<Vector3f, Vector3f, Vector3f> MakeOrthonormalBasis(const Vector3f& a, const Vector3f& b, const Vector3f& c);
}
