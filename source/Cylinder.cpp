#include "precompiled.h"
#include "Physics.h"
#include "Graphics.h"

namespace Framework
{

	
void CollisionCylinder::Serialize(ISerializer& stream)
{
	ISerializer* str = stream.clone();
	if(str->isGood())
	{
		float temp;
		str->readValue("Radius", temp);
		radius=(real)temp;
		
		str->readValue("Height", temp);
		height = (real)temp;
	}
	delete str;
}

Vector3 CollisionCylinder::Support(const Vector3 & dir)
{
	Vector3 dirNormal = dir;
	//dirNormal.normalise();
	dirNormal=transform.transformInverseDirection(dirNormal);
	Vector3 supportInLocalSpace(0,0,0);
	real w = sqrt(dirNormal.x*dirNormal.x + dirNormal.z*dirNormal.z);
    if (w > 0.0f)
    {
        supportInLocalSpace.x = dirNormal.x / w * radius;
		supportInLocalSpace.y = (dirNormal.y>0) ? 1:-1;
        supportInLocalSpace.y *= (height * 0.5);
        supportInLocalSpace.z = dirNormal.z / w * radius;
    }
    else
    {
        supportInLocalSpace.y = (dirNormal.y>0) ? 1:-1;
        supportInLocalSpace.y *= (height * 0.5);
    }
	return transform.transform(supportInLocalSpace);
}

real CollisionCylinder::getVolume()
{
	return (radius*radius*height*3.141593);
}

Matrix3 CollisionCylinder::getInettiaTensor(const real &mass)
{
	Matrix3 t;
	real rsquare,hsquare;
	rsquare = radius*radius;
	hsquare = height*height;
    t.setInertiaTensorCoeffs( 0.0833*mass*(3*rsquare + hsquare), 0.5*mass*rsquare, 0.0833*mass*(3*rsquare + hsquare) );
	return t;
}

real CollisionCylinder::getBoundingRadius()
{
	Vector3 halfSize(radius,height*0.5,radius);
	return sqrt((halfSize.x*2.0) * (halfSize.x*2.0) + (halfSize.y*2.0) * (halfSize.y*2.0) + (halfSize.z*2.0) * (halfSize.z*2.0)) * 0.5;
}

unsigned CollisionCylinder::rayCast(const Ray &ray,Vector3 &pointOfIntersection)
{
	Vector3 halfSize(radius,height*0.5,radius);
	Vector3 rayPos, rayDir;
	rayPos = transform.transformInverse(ray.position);
	rayDir = transform.transformInverseDirection(ray.direction);

	Vector3 min,max;
	min = halfSize*-1;
	max = halfSize;

	real t_min = 0.0;
	real t_max = REAL_MAX;

	for(int i=0;i<3;++i)
	{
		if(abs(rayDir[i])<EPSILON)
		{
			// Ray is parallel to slab. No hit if origin not within slab
			if(rayPos[i]<min[i] || rayPos[i]>max[i])
				return 0;
		}else
		{
			real d = 1.0/rayDir[i];
			real t1 = (min[i] - rayPos[i]) * d;
			real t2 = (max[i] - rayPos[i]) * d;

			if(t1>t2)
			{
				real temp = t1;
				t1 = t2;
				t2 = temp;
			}

			if(t1>t_min) t_min = t1;
			if(t2<t_max) t_max = t2;

			if(t_min>t_max)
				return 0;
		}
	}

	Vector3 point = rayPos + (rayDir*(t_min+EPSILON));
	pointOfIntersection = transform.transform(point);
	return 1;
}


}