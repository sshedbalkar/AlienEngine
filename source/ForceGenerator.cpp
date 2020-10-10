#include "precompiled.h"
#include "ForceGenerator.h"


namespace Framework
{

void Gravity::updateForce(RigidBody* body, real duration)
{
	if(!body->hasFiniteMass())return;
	body->addForce(gravity*body->getMass());
}

void Spring::updateForce(RigidBody* body, real duration)
{
    // Calculate the two ends in world space
    Vector3 lws = body->getPointInWorldSpace(connectionPoint);
    Vector3 ows = other->getPointInWorldSpace(otherConnectionPoint);

    Vector3 length = lws - ows;
	real magnitude = length.magnitude();
    magnitude = real_abs(magnitude - restLength);
    magnitude *= springConstant;

    // Calculate the final force and apply it
	Vector3 force = length;
	force.normalise();
    force *= -magnitude;
    body->addForceAtPoint(force, lws);
}

void ForceRegistry::add(RigidBody *body, ForceGenerator *fg)
{
    ForceRegistration registration;
    registration.body = body;
    registration.fg = fg;
    registrations.push_back(registration);
}

void ForceRegistry::remove(RigidBody *body, ForceGenerator *fg)
{
	Registry::iterator it=registrations.begin();
	for(;it!=registrations.end();++it)
	{
		if(it->fg==fg && it->body==body) break;
    }
	registrations.erase(it);
}

void ForceRegistry::updateForces(real dt)
{
	Registry::iterator i = registrations.begin();
    for (; i != registrations.end(); i++)
    {
		i->fg->updateForce(i->body,dt);
    }
}

}