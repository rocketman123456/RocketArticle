#include "Physics/Particle.h"

using namespace Rocket;

void Particle::Integrate(Real duration)
{
    // We don’t integrate things with infinite mass.
    if (inverse_mass_ <= 0.0f) return;
    assert(duration > 0.0); 
    
    // Update linear position.
    position_ = position_ + duration * velocity_; // + duration * duration / 2.0 * acceleration_;

    // Work out the acceleration from the force.
    // (We’ll add to this vector when we come to generate forces.) 
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