#pragma once
#include "Physics/Particle.h"

namespace Rocket
{
    class ParticleForceGenerator
    {
    public:
        virtual void UpdateForce(Particle* particle, Real duration) = 0;
    };

    class ParticleForceRegistry
    {
    public:
        void Add(Particle* particle, ParticleForceGenerator* fg);
        void Remove(Particle* particle, ParticleForceGenerator *fg); // TODO
        void Clear();
        void UpdateForces(Real duration);

    protected:
        struct ParticleForceRegistration 
        { 
            Particle* particle; 
            ParticleForceGenerator* force_generate; 
        };
        typedef Vec<ParticleForceRegistration> Registry;
        Registry registrations_;
    };

    class ParticleGravity : implements ParticleForceGenerator
    {
    public:
        ParticleGravity(const Real3& gravity) : gravity_(gravity) {}
        void UpdateForce(Particle* particle, Real duration) final;

    private:
        Real3 gravity_;
    };

    class ParticleDrag : implements ParticleForceGenerator
    {
    public:
        ParticleDrag(Real k1, Real k2) : k1_(k1), k2_(k2) {}
        void UpdateForce(Particle *particle, Real duration) final;
    private:
        Real k1_;
        Real k2_;
    };

    class ParticleSpring : implements ParticleForceGenerator
    {
    };
}
