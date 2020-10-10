#include"ParticleForceGenerator.h"
#include"ParticleContacts.h"

#ifndef PARTICLEWORLD_H
#define PARTICLEWORLD_H


namespace Framework
{


class ParticleWorld
{
public:
	typedef std::vector<Particle*> Particles;
    typedef std::vector<ParticleContactGenerator*> ContactGenerators;

protected:
	Particles particles;
	
	/**
    * True if the world should calculate the number of iterations
    * to give the contact resolver at each frame.
    */
	bool calculateIterations;

	ParticleForceRegistry registry;

	ParticleContactResolver resolver;

	ContactGenerators contactGenerators;

	ParticleContact *contacts;

	unsigned maxContacts;

public:
    
	ParticleWorld(unsigned maxContacts, unsigned iterations=0);
	~ParticleWorld();

    unsigned generateContacts();//goes thru the contact generators and generates contacts
    void integrate(real dt);
    void runPhysics(real dt);
	void startFrame();

    //Returns the list of particles.
    Particles& getParticles();

    //Returns the list of contact generators.
    ContactGenerators& getContactGenerators();
	    
    //Returns the force registry.
    ParticleForceRegistry& getForceRegistry();
};


class GroundContacts : public ParticleContactGenerator
{
    ParticleWorld::Particles *particles;
public:
    void init(ParticleWorld::Particles *particles);
	virtual unsigned addContact(ParticleContact *contact,unsigned limit) const;
};


}
#endif
