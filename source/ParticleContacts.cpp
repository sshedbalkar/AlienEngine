#include "ParticleContacts.h"


namespace Framework
{


void ParticleContact::resolve(real dt)
{
	resolveVelocity(dt);
	resolveInterpenetration(dt);
}

real ParticleContact::calculateSaperatingVelocity() const
{
	Vector3 vel1,vel2,relativeVelocity;
	vel1=vel2= Vector3(0.0,0.0,0.0);
	vel1= particle[0]->getVelocity();
	if(particle[1])
		vel2= particle[1]->getVelocity();
	relativeVelocity= vel1-vel2;

	return relativeVelocity*contactNormal ;
}

void ParticleContact::resolveVelocity(real dt)
{
	real separatingVelocity = calculateSaperatingVelocity();

	if(separatingVelocity>0)
		return;//no contact or they are relatively stationary
    
	real newSepVelocity = -separatingVelocity * restitution;

	//checking for resting contacts
	Vector3 accDiff;
	if(particle[1])
		accDiff=particle[0]->getAcceleration()-particle[1]->getAcceleration();
	else
		accDiff=particle[0]->getAcceleration();
	real accelerationCausedImpulse= accDiff*contactNormal*dt;
			//remove this impulse

	if(accelerationCausedImpulse <0)
	{
		newSepVelocity+= restitution*accelerationCausedImpulse;
		//clamp this to 0
		if(newSepVelocity<0) 
			newSepVelocity=0;
	}

	real deltaVelocity = newSepVelocity - separatingVelocity;

    // We apply the change in velocity to each object in proportion to
    // their inverse mass (i.e. those with lower inverse mass [higher
    // actual mass] get less change in velocity)..
    real totalInverseMass = particle[0]->getInverseMass();
    if (particle[1]) totalInverseMass += particle[1]->getInverseMass();

    // If all particles have infinite mass, then impulses have no effect
    if (totalInverseMass <= 0) return;

	// Calculate the impulse to apply
    real impulse = deltaVelocity / totalInverseMass;

    // Find the amount of impulse per unit of inverse mass
    Vector3 impulsePerIMass = contactNormal * impulse;

	particle[0]->setVelocity(particle[0]->getVelocity()+impulsePerIMass * particle[0]->getInverseMass());
	if (particle[1])
        particle[1]->setVelocity(particle[1]->getVelocity()+impulsePerIMass * -particle[1]->getInverseMass());
    
}

void ParticleContact::resolveInterpenetration(real dt)
{
    if (penetration <= 0) return;

	real totalInverseMass = particle[0]->getInverseMass();
    if (particle[1]) totalInverseMass += particle[1]->getInverseMass();
    if (totalInverseMass <= 0) return;

    Vector3 movePerIMass = contactNormal * (penetration / totalInverseMass);
	particleMovement[0] = movePerIMass * particle[0]->getInverseMass();
	if(particle[1])
		particleMovement[1] = movePerIMass * particle[1]->getInverseMass();
	else
		particleMovement[1].clear();

    particle[0]->setPosition(particle[0]->getPosition() + particleMovement[0]);
	if(particle[1])
        particle[1]->setPosition(particle[1]->getPosition() + particleMovement[1]);
    
}

void ParticleContactResolver::resolveContacts(ParticleContact *contactArray, unsigned numContacts, real dt)
{
	unsigned i;
	real max;
	iterationsUsed=0;
	while(iterationsUsed<iterations)
	{
		//find the contact with the largest closing velocity
		max=REAL_MAX;
		unsigned maxIndex=numContacts;
		for (i = 0; i < numContacts; i++)
        {
			real sepVel = contactArray[i].calculateSaperatingVelocity();
            if (sepVel < max && (sepVel < 0 || contactArray[i].penetration > 0))
            {
                max = sepVel;
                maxIndex = i;
            }
        }

		// Do we have anything worth resolving?
        if (maxIndex == numContacts) break;

        // Resolve this contact
        contactArray[maxIndex].resolve(dt);

		// Update the interpenetrations for all particles
        Vector3 *move = contactArray[maxIndex].particleMovement;
        for (i = 0; i < numContacts; i++)
        {
            if (contactArray[i].particle[0] == contactArray[maxIndex].particle[0])
            {
                contactArray[i].penetration -= move[0] * contactArray[i].contactNormal;
            }
            else if (contactArray[i].particle[0] == contactArray[maxIndex].particle[1])
            {
                contactArray[i].penetration -= move[1] * contactArray[i].contactNormal;
            }
            if (contactArray[i].particle[1])
            {
                if (contactArray[i].particle[1] == contactArray[maxIndex].particle[0])
                {
                    contactArray[i].penetration += move[0] * contactArray[i].contactNormal;
                }
                else if (contactArray[i].particle[1] == contactArray[maxIndex].particle[1])
                {
                    contactArray[i].penetration += move[1] * contactArray[i].contactNormal;
                }
            }
        }



		iterationsUsed++;
	}
}

}