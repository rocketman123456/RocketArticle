#pragma once
#include "Core/Core.h"
#include "Common/GeomMath.h"

namespace Rocket
{
	struct Ray
	{
        Ray(const Point3D& orig, const Vector3f& dir, double time = 0.0)
            : origin(orig), direction(dir), tm(time) {}
        Ray() = default;
        ~Ray() = default;

        Point3D Origin() const { return origin; }
        Vector3f Direction() const { return direction; }
        double Time() const { return tm; }
        Point3D At(double t) const { return origin + t * direction; }

		Point3D origin;
		Vector3f direction;
		double tm;
	};
}