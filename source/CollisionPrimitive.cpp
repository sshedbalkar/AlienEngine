#include "precompiled.h"
#include "Physics.h"
#include "Graphics.h"

namespace Framework
{

	

void CollisionPrimitive::Initialize()
{
	body=GetOwner()->has(RigidBody);
	t=GetOwner()->has(Transform);
	//calculateInternals();
	aabb = new AABB(body);
	aabb->center= t->Position;
	Quaternion q=t->QUATERNION;
	q.normalise();
	Vector3 size = t->Scale * 0.5;
	Matrix4 qwe;
	qwe.setOrientationAndPos(q,t->Position);
	aabb->halfSize = qwe.transformOOBBtoAABB(size);
	if(body)
		PHYSICS->SortedAABBArrayX.push_back(aabb);
	calculateInternalsinEditor();
}

void CollisionPrimitive::RemoveComponentFromGame()
{
	std::vector<AABB*> ::iterator it = PHYSICS->SortedAABBArrayX.begin();
	for (;it<PHYSICS->SortedAABBArrayX.end();++it)
	{
		if(this->aabb == (*it))
		{
			(*it)=PHYSICS->SortedAABBArrayX.back();
			PHYSICS->SortedAABBArrayX.pop_back();
			break;
		}
	}
}

void CollisionPrimitive::RestoreComponentToGame()
{
	PHYSICS->SortedAABBArrayX.push_back(this->aabb);
}


void CollisionPrimitive::Serialize(ISerializer& stream)
{

}


void CollisionPrimitive::SendMessage(Message *m)
{
	switch( m->MessageId )
		{
			
		}
}


void CollisionPrimitive::calculateInternals()
{
	if(body)
		transform = body->getTransform()*offset;
	if(aabb)
	{
		aabb->halfSize= transform.transformOOBBtoAABB(t->Scale * 0.5);
		aabb->center=getAxis(3);//pos
		aabb->min[0] = aabb->center.x - aabb->halfSize.x;
		aabb->max[0] = aabb->center.x + aabb->halfSize.x;
		aabb->min[1] = aabb->center.z - aabb->halfSize.z;
		aabb->max[1] = aabb->center.z + aabb->halfSize.z;
	}
}

void CollisionPrimitive::calculateInternalsinEditor()
{
	Quaternion q(GetOwner()->has(Transform)->QUATERNION.w,GetOwner()->has(Transform)->QUATERNION.x,GetOwner()->has(Transform)->QUATERNION.y,GetOwner()->has(Transform)->QUATERNION.z);
	q.normalise();
	Vector3 pos(GetOwner()->has(Transform)->Position.x,GetOwner()->has(Transform)->Position.y,GetOwner()->has(Transform)->Position.z);
	transform.setOrientationAndPos(q,pos);
	transform = transform*offset;
}


}