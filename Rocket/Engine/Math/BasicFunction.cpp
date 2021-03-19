#include "Math/BasicFunction.h"

namespace Rocket
{
    std::tuple<Vector3f, Vector3f, Vector3f> MakeOrthonormalBasis(const Vector3f& a, const Vector3f& b, const Vector3f& c)
    {
        auto _a = a.normalized();
        auto _c = _a.cross(b);
        if(_c.squaredNorm() == 0.0) return {};
        _c.normalize();
        auto _b = _c.cross(_a);
        return {_a, _b, _c};
    }
}