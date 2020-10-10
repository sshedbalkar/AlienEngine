#include "ParticleWorld.h"

namespace Framework
{

ParticleWorld::ParticleWorld(unsigned maxContacts, unsigned iterations):resolver(iterations),maxContacts(maxContacts)
{
    contacts = new ParticleContact[maxContacts];
    calculateIterations = (iterations == 0);
}

ParticleWorld::~ParticleWorld()
{
    delete[] contacts;
}

void ParticleWorld::startFrame()
{
    for(Particles::iterator p = particles.begin();p != particles.end();p++)
		(*p)->clearAccumulator();
}

unsigned ParticleWorld::generateContacts()
{
    unsigned limit = maxContacts;
    ParticleContact *nextContact = contacts;

	ContactGenerators::iterator g = contactGenerators.begin();
    for(;g != contactGenerators.end();g++)
    {
        unsigned used =(*g)->addContact(nextContact, limit);
        limit -= used;
        nextContact += used;

        if (limit <= 0) 
			break;
    }

    // Return the number of contacts used.
    return maxContacts - limit;
}

void ParticleWorld::integrate(real dt)
{
    for (Particles::iterator p = particles.begin();p != particles.end();p++)
		(*p)->integrate(dt);
}

ParticleWorld::Particles& ParticleWorld::getParticles()
{
    return particles;
}

ParticleWorld::ContactGenerators& ParticleWorld::getContactGenerators()
{
    return contactGenerators;
}

ParticleForceRegistry& ParticleWorld::getForceRegistry()
{
    return registry;
}

void ParticleWorld::runPhysics(real dt)
{
    // First apply the force generators
    registry.updateForces(dt);

    // Then integrate the objects
    integrate(dt);

    // Generate contacts
    unsigned usedContacts = generateContacts();

    // And process them
    if (usedContacts)
    {
        if (calculateIterations) 
			resolver.setIterations(usedContacts * 2);
        resolver.resolveContacts(contacts, usedContacts, dt);
    }
}

void GroundContacts::init(ParticleWorld::Particles *particles)
{
    GroundContacts::particles = particles;
}


unsigned GroundContacts::addContact(ParticleContact *contact,unsigned limit) const
{
    unsigned count = 0;
	ParticleWorld::Particles::iterator p = particles->begin();
    for(;p != particles->end();p++)
    {
       real y = (*p)->getPosition().y;
        if (y < 0.0f)
        {
            contact->contactNormal = Vector3(0,1,0);
            contact->particle[0] = (*p);
            contact->particle[1] = NULL;
            contact->penetration = -y;
            contact->restitution = 0.2f;
            contact++;
            count++;
        }

        if (count >= limit) 
			return count;
    }
    return count;
}


}