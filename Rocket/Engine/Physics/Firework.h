#pragma once
#include "Physics/Particle.h"

namespace Rocket
{
    class Firework : implements Particle
    {
    public:
        void SetType(uint32_t type) { type_ = type; }
        void SetAge(Real age) { age_ = age; }

        const uint32_t GetType() const { return type_; }
        const Real GetAge() const { return age_; }

        bool Update(Real duration);

    protected:
        uint32_t type_;
        Real age_;
    };

    // Firework rules control the length of a firework’s fuse and the particles it should evolve into.
    struct FireworkRule
    {
        uint32_t type;
        Real min_age;
        Real max_age;
        Real3 min_velocity;
        Real3 max_velocity;
        Real damping;

        // The payload is the new firework type to create when this firework’s fuse is over.
        struct Payload
        {
            uint32_t type;
            uint32_t count;

            void Set(uint32_t type, uint32_t count) 
            { 
                Payload::type = type; 
                Payload::count = count; 
            }
        };

        // TODO : maybe use std::vector instead
        uint32_t payload_count;
        Payload *payloads; // Set of Payloads

        FireworkRule() : payload_count(0), payloads(nullptr) {}
        ~FireworkRule() { if (payloads != nullptr) delete[] payloads; }

        void Initialize(unsigned payload_count);
        void SetParameters(uint32_t type, Real min_age, Real max_age, const Real3& min_vel, const Real3 &max_vel, Real damping);
        void Create(Firework *firework, const Firework *parent = nullptr) const;
    };
}
