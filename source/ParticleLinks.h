#include "ParticleContacts.h"

#ifndef PARTICLELINKS_H
#define PARTICLELINKS_H

namespace Framework
{


class ParticleLink:public ParticleContactGenerator
{
protected:
	real currentLength() const;
public:
	Particle *particle[2];
	virtual unsigned addContact(ParticleContact *contact,unsigned limit) const = 0;
};

/**
* Cables link a pair of particles, generating a contact if they
* stray too far apart.
*/
class ParticleCable : public ParticleLink
{
public:
    real maxLength;
	real restitution;
public:
    /**
    * Fills the given contact structure with the contact needed
    * to keep the cable from over-extending.
    */
    virtual unsigned addContact(ParticleContact *contact,unsigned limit) const;
};

/**
* Rods link a pair of particles, generating a contact if they
* stray too far apart or too close.
*/
class ParticleRod : public ParticleLink
{
public:
    real length;

public:
    /**
    * Fills the given contact structure with the contact needed
    * to keep the rod from extending or compressing.
    */
    virtual unsigned addContact(ParticleContact *contact,unsigned limit) const;
};


class ParticleConstraint:public ParticleContactGenerator
{
protected:
	real currentLength() const;
public:
	Particle *particle;
	Vector3 anchor;
	virtual unsigned addContact(ParticleContact *contact,unsigned limit) const = 0;
};

/**
* Cables link a particle to an anchor point, generating a contact if they
* stray too far apart.
*/
class ParticleCableConstraint : public ParticleConstraint
{
public:
    real maxLength;
	real restitution;

public:
    virtual unsigned addContact(ParticleContact *contact,unsigned limit) const;
};

/**
* Rods link a particle to an anchor point, generating a contact if they
* stray too far apart or too close.
*/
class ParticleRodConstraint : public ParticleConstraint
{
public:
    real length;

public:
    virtual unsigned addContact(ParticleContact *contact,unsigned limit) const;
};


}

#endif