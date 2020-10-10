#include "Particle.h"
#include "PhysicsMath.h"
#include <vector>
#include <list>

#ifndef PARTICLEFORCEGENERATOR_H
#define PARTICLEFORCEGENERATOR_H


namespace Framework
{

//virtual class
class ParticleForceGenerator
{
public:
    virtual void updateForce(Particle *particle, real dt) = 0;
};

class ParticleGravity : public ParticleForceGenerator
{
    Vector3 gravity;
public:
	ParticleGravity(const Vector3 &g):gravity(g){}
	virtual void updateForce(Particle *particle, real dt);
};

class ParticleDrag : public ParticleForceGenerator
{
	real k1;
	real k2;
public:
	ParticleDrag(real k1,real k2):k1(k1),k2(k2) {}
	virtual void updateForce(Particle *particle, real dt);
};

class ParticleAnchoredSpring : public ParticleForceGenerator
{
protected:
    Vector3 *anchor;
	real springConstant;
	real restLength;
public:
	ParticleAnchoredSpring(){}
	ParticleAnchoredSpring(Vector3 *anchor,real springConstant,real restLength):
		anchor(anchor), springConstant(springConstant),restLength(restLength) {}
    const Vector3* getAnchor() const { return anchor; }
	void init(Vector3 *anchor,real springConstant,real restLength);
	virtual void updateForce(Particle *particle, real duration);
};

class ParticleAnchoredBungee : public ParticleAnchoredSpring
{
public:
    virtual void updateForce(Particle *particle, real duration);
};

class ParticleFakeSpring : public ParticleForceGenerator
{
    Vector3 *anchor;
	real springConstant;
	real damping;
public:

    ParticleFakeSpring(Vector3 *anchor, real springConstant,real damping):
	  anchor(anchor), springConstant(springConstant), damping(damping) {}
    virtual void updateForce(Particle *particle, real duration);
};


class ParticleSpring : public ParticleForceGenerator
{
    Particle *other;
    real springConstant;
    real restLength;
public:
    ParticleSpring(Particle *other,real springConstant, real restLength):
	  other(other), springConstant(springConstant), restLength(restLength) {}
    virtual void updateForce(Particle *particle, real duration);
};

class ParticleBungee : public ParticleForceGenerator
{
    Particle *other;
    real springConstant;
    real restLength;
public:
    ParticleBungee(Particle *other,real springConstant, real restLength):
	  other(other), springConstant(springConstant), restLength(restLength) {}
    virtual void updateForce(Particle *particle, real duration);
};

//buoyancy considering tht water is at y=0
class ParticleBuoyancy : public ParticleForceGenerator
{
    real maxDepth;
    real volume;
    real waterHeight;
    real liquidDensity;
public:
    ParticleBuoyancy(real maxDepth, real volume, real waterHeight,real liquidDensity = 1000.0f) :
	  maxDepth(maxDepth), volume(volume), waterHeight(waterHeight), liquidDensity(liquidDensity) {}
    virtual void updateForce(Particle *particle, real duration);
};

class ParticleForceRegistry
{
protected:
    struct ParticleForceRegistration
    {
        Particle *particle;
        ParticleForceGenerator *fg;
    };
	typedef std::list<ParticleForceRegistration> Registry;
    Registry registrations;

public:
    void add(Particle* particle, ParticleForceGenerator *fg);
	void remove(Particle* particle, ParticleForceGenerator *fg);
	void clear(){registrations.clear();}
	void updateForces(real duration);
};


}

#endif
