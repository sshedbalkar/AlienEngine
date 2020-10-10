#include "Contacts.h"

#ifndef JOINTS_H
#define JOINTS_H


namespace Framework
{
	
	class Joint : public ContactGenerator
    {
    public:
        RigidBody* body[2];
        Vector3 position[2];
		Vector3 worldPosition[2];//for debug only
        real error;
        void set(RigidBody *a, const Vector3& a_pos,RigidBody *b, const Vector3& b_pos,real error);
        virtual unsigned addContact(Contact *contact);
    };

	class Constraint : public ContactGenerator
	{
	public:
		RigidBody* body;
        Vector3 position;
        Vector3 worldPosition;
		Vector3 bodyWorldPosition;//for debug only
        real error;
        void set(RigidBody *a, const Vector3& a_pos, const Vector3& worldPos,real err);
        virtual unsigned addContact(Contact *contact);
	};

}
#endif