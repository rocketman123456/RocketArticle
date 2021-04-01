#include "Physics/ParticleForce.h"

using namespace Rocket;

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

void ParticleDrag::UpdateForce(Particle* particle, Real duration) 
{ 
    Real3 force; 
    // Calculate the total drag coefficient.
    force = particle->GetVelocity();
    Real dragCoeff = force.norm(); 
    // Calculate the final force and apply it.
    dragCoeff = k1_ * dragCoeff + k2_ * dragCoeff * dragCoeff;
    force.normalize(); 
    force *= -dragCoeff; 
    particle->AddForce(force); 
}