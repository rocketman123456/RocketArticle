#include "Physics/Firework.h"

using namespace Rocket;

static Rocket::Random crandom;

bool Firework::Update(Real duration)
{ 
    // Update our physical state.
    Integrate(duration); 
    // We work backward from our age to zero.
    age_ -= duration; 
    return (age_ < 0) || (position_[0] < 0); 
}

void FireworkRule::Initialize(unsigned payload_count)
{
    FireworkRule::payload_count = payload_count;
    payloads = new Payload[payload_count];
}

void FireworkRule::SetParameters(uint32_t type, Real min_age, Real max_age, const Real3& min_vel, const Real3 &max_vel, Real damping)
{
    FireworkRule::type = type;
    FireworkRule::min_age = min_age;
    FireworkRule::max_age = max_age;
    FireworkRule::min_velocity = min_vel;
    FireworkRule::max_velocity = max_vel;
    FireworkRule::damping = damping;
}

void FireworkRule::Create(Firework* firework, const Firework* parent) const
{
    firework->SetType(type);
    firework->SetAge(crandom.RandomReal(min_age, max_age));

    Real3 vel;
    if (parent)
    {
        // The position and velocity are based on the parent.
        firework->SetPosition(parent->GetPosition());
        vel += parent->GetVelocity();
    }
    else
    {
        Real3 start;
        int x = (int)crandom.RandomInt(3) - 1;
        start[0] = 5.0f * Real(x);
        firework->SetPosition(start);
    }

    vel += crandom.RandomVector(min_velocity, max_velocity);
    firework->SetVelocity(vel);

    // We use a mass of one in all cases (no point having fireworks
    // with different masses, since they are only under the influence
    // of gravity).
    firework->SetMass(1);
    firework->SetDamping(damping);
    firework->SetAcceleration(Real3(0, 0, -9.81));
    firework->ClearAccumulator();
}