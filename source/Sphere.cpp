#include "precompiled.h"
#include "Physics.h"
#include "Graphics.h"

namespace Framework
{

	
void CollisionSphere::Serialize(ISerializer& stream)
{
	ISerializer* str = stream.clone();
	if(str->isGood())
	{
		float rad;
		str->readValue("Radius", rad);
		radius=(real)rad;
	}
	delete str;
}


Vector3 CollisionSphere::Support(const Vector3 & dir)
{
	Vector3 dirNormal = dir;
	//dirNormal.normalise();
	dirNormal=transform.transformInverseDirection(dirNormal);
	Vector3 supportInLocalSpace = dirNormal*radius;
	return transform.transform(supportInLocalSpace);
}

real CollisionSphere::getVolume()
{
	return (radius*radius*radius*(4.0/3.0)*3.141593);
}

Matrix3 CollisionSphere::getInettiaTensor(const real &mass)
{
	Matrix3 t;
	real temp=0.4*mass*radius*radius;
	t.setInertiaTensorCoeffs(temp,temp,temp);
	return t;
}


unsigned CollisionSphere::rayCast(const Ray &ray,Vector3 &pointOfIntersection)
{
	Vector3 centre = getAxis(3);
	Vector3 centretoRay = ray.position-centre;

	real v = centretoRay*ray.direction;
	real c = (centretoRay*centretoRay) - (radius*radius); 
	if(c>0.0f && v>0.0f) 
		return 0;

	real distance = v*v -c;
	if(distance<0.0f)
		return 0;
	else
	{
		real d=-v-sqrt(distance);
		if(d<0.0f)d=0.0;
		pointOfIntersection = ray.position + ray.direction*d;
		return 1;
	}
}


}