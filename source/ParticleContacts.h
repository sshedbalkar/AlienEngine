#include "Particle.h"

#ifndef PARTICLECONTACTS_H
#define PARTICLECONTACTS_H


namespace Framework
{

class ParticleContactResolver;

class ParticleContact
{

    friend ParticleContactResolver;

public:
	Particle *particle[2];
	real restitution;
	Vector3 contactNormal;
	real penetration;

	Vector3 particleMovement[2];

protected:
	void resolve(real dt);
	real calculateSaperatingVelocity() const;

public:
	void resolveVelocity(real dt);
	void resolveInterpenetration(real dt);
};

class ParticleContactResolver
{
protected:
	unsigned iterations;
	unsigned iterationsUsed;
public:
	ParticleContactResolver(unsigned it):iterations(it){}
	void setIterations(unsigned it){iterations=it;}
	void resolveContacts(ParticleContact *contactArray, unsigned numContacts, real dt);
};

class ParticleContactGenerator
{
public:
    virtual unsigned addContact(ParticleContact *contact,unsigned limit) const = 0;
};


}

#endif