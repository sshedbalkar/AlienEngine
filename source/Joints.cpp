#include "Joints.h"

namespace Framework
{
	void Joint::set(RigidBody *a, const Vector3& a_pos,RigidBody *b, const Vector3& b_pos,real err)
	{
		body[0] = a;
		body[1] = b;
		position[0] = a_pos;
		position[1] = b_pos;
		error=err;
	}

	unsigned Joint::addContact(Contact *contact)
	{
		Vector3 a_pos_world = body[0]->getPointInWorldSpace(position[0]);
		Vector3 b_pos_world = body[1]->getPointInWorldSpace(position[1]);

		Vector3 a_to_b = b_pos_world - a_pos_world;
		Vector3 normal = a_to_b;
		normal.normalise();
		real length = a_to_b.magnitude();

		worldPosition[0]=a_pos_world;
		worldPosition[1]=b_pos_world;
		if(!body[0]->getAwake() && !body[1]->getAwake()) return 0;
		
		if (real_abs(length) > error)
		{
			contact->body[0] = body[0];
			contact->body[1] = body[1];
			contact->contactNormal = normal;
			contact->contactPoint = (a_pos_world + b_pos_world) * 0.5f;
			contact->penetration = length-error;
			contact->friction = 1.0f;
			contact->restitution = 0;
			return 1;
		}		

		return 0;
	}

	void Constraint::set(RigidBody *a, const Vector3& a_pos, const Vector3& worldPos,real err)
	{
		body = a;
		position = a_pos;
		worldPosition = worldPos;
		error=err;
	}


	unsigned Constraint::addContact(Contact *contact)
	{
		Vector3 a_pos_world = body->getPointInWorldSpace(position);

		Vector3 a_to_p = worldPosition - a_pos_world;
		Vector3 normal = a_to_p;
		normal.normalise();
		real length = a_to_p.magnitude();

		bodyWorldPosition=a_pos_world;
		if(!body->getAwake()) return 0;

		if (real_abs(length) > error)
		{
			contact->body[0] = body;
			contact->body[1] = NULL;
			contact->contactNormal = normal;
			contact->contactPoint = (a_pos_world + worldPosition) * 0.5f;
			contact->penetration = length-error;
			contact->friction = 1.0f;
			contact->restitution = 0;
			return 1;
		}
		return 0;
	}


}