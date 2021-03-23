#include "Physics/Particle.h"

using namespace Rocket;

void Particle::Integrate(Real duration)
{
    // We don’t integrate things with infinite mass.
    if (inverse_mass_ <= 0.0f) return;
    assert(duration > 0.0); 
    
    // Update linear position.
    position_ = position_ + duration * velocity_; // + duration * duration / 2.0 * acceleration_;

    // Work out the acceleration from the force
    Vector3d resultingAcc = acceleration_; 
    // Update linear velocity from the acceleration.
    velocity_ = velocity_ + duration * resultingAcc;
    // Impose drag.
    velocity_ *= RealPow(damping_, duration); 
    // Clear the forces.
    ClearAccumulator();
}

void Particle::ClearAccumulator()
{
    force_accumulate_ << 0, 0, 0;
}

void ParticleForceRegistry::UpdateForces(Real duration)
{
    for (auto i = registrations_.begin(); i != registrations_.end(); ++i) 
    { 
        i->force_generate->UpdateForce(i->particle, duration); 
    }
}

void ParticleForceRegistry::Add(Particle* particle, ParticleForceGenerator* fg)
{
    ParticleForceRegistry::ParticleForceRegistration registration;
    registration.particle = particle;
    registration.force_generate = fg;
    registrations_.push_back(registration);
}

void ParticleForceRegistry::Clear()
{
    registrations_.clear();
}

void ParticleGravity::UpdateForce(Particle* particle, Real duration)
{
    if (!particle->HasFiniteMass()) return;
    // Apply the mass-scaled force to the particle.
    particle->AddForce(gravity_ * particle->GetMass());
}
