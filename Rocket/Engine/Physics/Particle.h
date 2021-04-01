#pragma once
#include "Math/GeomMath.h"

namespace Rocket
{
    // TODO : make z upward direction
    class Particle
    {
    public:
        void Integrate(Real duration);
        void ClearAccumulator();

        void SetPosition(Real x, Real y, Real z) { position_ << x, y, z; }
        void SetPosition(const Real3& pos) { position_ = pos; }
        void SetVelocity(Real x, Real y, Real z) { velocity_ << x, y, z; }
        void SetVelocity(const Real3& vel) { velocity_ = vel; }
        void SetAcceleration(Real x, Real y, Real z) { acceleration_ << x, y, z; }
        void SetAcceleration(const Real3& acc) { acceleration_ = acc; }
        void SetDamping(float d) { damping_ = d; }
        void SetMass(float mass) { inverse_mass_ = 1.0 / mass; }
        void SetInverseMass(float im) { inverse_mass_ = im; }
        void AddForce(const Real3& f) { force_accumulate_ += f; }

        const Real3& GetPosition() const { return position_; }
        const Real3& GetVelocity() const { return velocity_; }
        const Real3& GetAcceleration() const { return acceleration_; }
        const Real3& GetForceAccumulate() const { return position_; }
        const float GetDamping() const { return damping_; }
        const float GetInverseMass() const { return inverse_mass_; }
        const float GetMass() const { return 1.0 / inverse_mass_; }

        bool HasFiniteMass() const { return inverse_mass_ >= 0.0; }

    protected:
        Real3 position_;
        Real3 velocity_;
        Real3 acceleration_;
        Real3 force_accumulate_;
        Real damping_;
        Real inverse_mass_;
    };
}